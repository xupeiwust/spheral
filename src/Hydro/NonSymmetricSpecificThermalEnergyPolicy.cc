//---------------------------------Spheral++----------------------------------//
// NonSymmetricSpecificThermalEnergyPolicy -- An implementation of 
// UpdatePolicyBase specialized for the updating the specific thermal energy
// as a dependent quantity.
// 
// This version is specialized for the compatible energy discretization 
// method in the case where we do *not* assume that pairwise forces are
// equal and opposite.
//
// Created by JMO, Sat Aug 10 23:03:39 PDT 2013
//----------------------------------------------------------------------------//
#include <vector>

#include "NonSymmetricSpecificThermalEnergyPolicy.hh"
#include "HydroFieldNames.hh"
#include "entropyWeightingFunction.hh"
#include "NodeList/NodeList.hh"
#include "NodeList/FluidNodeList.hh"
#include "DataBase/DataBase.hh"
#include "DataBase/FieldListUpdatePolicyBase.hh"
#include "DataBase/IncrementFieldList.hh"
#include "DataBase/ReplaceState.hh"
#include "DataBase/State.hh"
#include "DataBase/StateDerivatives.hh"
#include "Neighbor/ConnectivityMap.hh"
#include "Field/Field.hh"
#include "Field/FieldList.hh"
#include "Utilities/DBC.hh"
#include "Utilities/safeInv.hh"
#include "Utilities/SpheralFunctions.hh"

namespace Spheral {

using namespace std;
using DataBaseSpace::DataBase;
using FieldSpace::Field;
using FieldSpace::FieldList;
using NodeSpace::NodeList;
using NodeSpace::FluidNodeList;
using NeighborSpace::ConnectivityMap;

// namespace {

// inline double weighting(const double wi,
//                         const double wj,
//                         const double Eij) {
//   const int si = isgn0(wi);
//   const int sj = isgn0(wj);
//   const int sij = isgn0(wij);
//   if (sij == 0 or (si == 0 and sj == 0))             return 0.5;
//   if (si == sj and sj == sij)                        return wi/(wi + wj);  // All the same sign and non-zero
//   if (si == sj)                                      return 0.5;
//   if (si == sij)                                     return 1.0;
//   return 0.0;
// }

// }

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
template<typename Dimension>
NonSymmetricSpecificThermalEnergyPolicy<Dimension>::
NonSymmetricSpecificThermalEnergyPolicy(const DataBase<Dimension>& dataBase):
  IncrementFieldList<Dimension, typename Dimension::Scalar>(),
  mDataBasePtr(&dataBase) {
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
template<typename Dimension>
NonSymmetricSpecificThermalEnergyPolicy<Dimension>::
~NonSymmetricSpecificThermalEnergyPolicy() {
}

//------------------------------------------------------------------------------
// Update the field.
//------------------------------------------------------------------------------
template<typename Dimension>
void
NonSymmetricSpecificThermalEnergyPolicy<Dimension>::
update(const KeyType& key,
       State<Dimension>& state,
       StateDerivatives<Dimension>& derivs,
       const double multiplier,
       const double t,
       const double dt) {

  typedef typename Dimension::SymTensor SymTensor;

//   // HACK!
//   std::cerr.setf(std::ios::scientific, std::ios::floatfield);
//   std::cerr.precision(15);

  KeyType fieldKey, nodeListKey;
  StateBase<Dimension>::splitFieldKey(key, fieldKey, nodeListKey);
  REQUIRE(fieldKey == HydroFieldNames::specificThermalEnergy and 
          nodeListKey == UpdatePolicyBase<Dimension>::wildcard());
  FieldList<Dimension, Scalar> eps = state.fields(fieldKey, Scalar());
  const unsigned numFields = eps.numFields();

  // Get the state fields.
  const FieldList<Dimension, Scalar> mass = state.fields(HydroFieldNames::mass, Scalar());
  const FieldList<Dimension, Vector> velocity = state.fields(HydroFieldNames::velocity, Vector::zero);
  const FieldList<Dimension, Vector> acceleration = derivs.fields(IncrementFieldList<Dimension, Vector>::prefix() + HydroFieldNames::velocity, Vector::zero);
  const FieldList<Dimension, Scalar> eps0 = state.fields(HydroFieldNames::specificThermalEnergy + "0", Scalar());
  const FieldList<Dimension, Scalar> entropy = state.fields(HydroFieldNames::entropy, Scalar());
  const FieldList<Dimension, vector<Vector> > pairAccelerations = derivs.fields(HydroFieldNames::pairAccelerations, vector<Vector>());
  // const FieldList<Dimension, Scalar> DepsDt0 = derivs.fields(IncrementFieldList<Dimension, Field<Dimension, Scalar> >::prefix() + HydroFieldNames::specificThermalEnergy, 0.0);
  const ConnectivityMap<Dimension>& connectivityMap = mDataBasePtr->connectivityMap();
  const vector<const NodeList<Dimension>*>& nodeLists = connectivityMap.nodeLists();
  CHECK(nodeLists.size() == numFields);

  // Check if there is a surface point flag field registered.  If so, we use non-compatible energy evolution 
  // on such points.
  const bool surface = state.fieldNameRegistered(HydroFieldNames::surfacePoint);
  FieldList<Dimension, int> surfacePoint;
  if (surface) {
    surfacePoint = state.fields(HydroFieldNames::surfacePoint, int(0));
  }

  // Prepare a counter to keep track of how we go through the pair-accelerations.
  FieldList<Dimension, Scalar> DepsDt = mDataBasePtr->newFluidFieldList(0.0, "delta E");
  FieldList<Dimension, int> offset = mDataBasePtr->newFluidFieldList(0, "offset");

  // Walk all the NodeLists.
  const double hdt = 0.5*multiplier;
  for (size_t nodeListi = 0; nodeListi != numFields; ++nodeListi) {

    // Iterate over the internal nodes of this NodeList.
    for (typename ConnectivityMap<Dimension>::const_iterator iItr = connectivityMap.begin(nodeListi);
         iItr != connectivityMap.end(nodeListi);
         ++iItr) {
      const int i = *iItr;

      // State for node i.
      Scalar& DepsDti = DepsDt(nodeListi, i);
      // const Scalar DepsDt0i = DepsDt0(nodeListi, i);
      const Scalar mi = mass(nodeListi, i);
      const Scalar si = entropy(nodeListi, i);
      const Vector& vi = velocity(nodeListi, i);
      const Scalar ui = eps0(nodeListi, i);
      const Vector& ai = acceleration(nodeListi, i);
      const Vector vi12 = vi + ai*hdt;
      const vector<Vector>& pacci = pairAccelerations(nodeListi, i);
      CHECK(pacci.size() == connectivityMap.numNeighborsForNode(nodeLists[nodeListi], i) or
            pacci.size() == connectivityMap.numNeighborsForNode(nodeLists[nodeListi], i) + 1);

      // Get the connectivity (neighbor set) for this node.
      const vector< vector<int> >& fullConnectivity = connectivityMap.connectivityForNode(nodeListi, i);

      // Iterate over the neighbor NodeLists.
      for (size_t nodeListj = 0; nodeListj != numFields; ++nodeListj) {

        // The set of neighbors from this NodeList.
        const vector<int>& connectivity = fullConnectivity[nodeListj];
        if (connectivity.size() > 0) {
          const int firstGhostNodej = nodeLists[nodeListj]->firstGhostNode();

          // Iterate over the neighbors, and accumulate the specific energy
          // change.
          for (vector<int>::const_iterator jitr = connectivity.begin();
               jitr != connectivity.end();
               ++jitr) {
            const int j = *jitr;

            if (connectivityMap.calculatePairInteraction(nodeListi, i, 
                                                         nodeListj, j,
                                                         firstGhostNodej)) {
              Scalar& DepsDtj = DepsDt(nodeListj, j);
              // const Scalar DepsDt0j = DepsDt0(nodeListj, j);
              const Scalar mj = mass(nodeListj, j);
              const Scalar sj = entropy(nodeListj, j);
              const Vector& vj = velocity(nodeListj, j);
              const Scalar uj = eps0(nodeListj, j);
              const Vector& aj = acceleration(nodeListj, j);
              const Vector vj12 = vj + aj*hdt;
              const vector<Vector>& paccj = pairAccelerations(nodeListj, j);
              // CHECK(j >= firstGhostNodej or paccj.size() == connectivityMap.numNeighborsForNode(nodeLists[nodeListj], j));

              CHECK(offset(nodeListi, i) < pacci.size());
              const Vector& pai = pacci[offset(nodeListi, i)];
              ++offset(nodeListi, i);

              CHECK(offset(nodeListj, j) < paccj.size());
              const Vector& paj = paccj[offset(nodeListj, j)];
              ++offset(nodeListj, j);

              const Scalar dEij = -(mi*vi12.dot(pai) + mj*vj12.dot(paj));
              const Scalar duij = dEij/mi;

              // const Scalar wi = paj.magnitude()*safeInv(pai.magnitude() + paj.magnitude());
              // const Scalar wi = (isgn0(DepsDt0i + DepsDt0j) != 0 ?
              //                    DepsDt0i/(DepsDt0i + DepsDt0j) :
              //                    entropyWeighting(si, sj, duij));
              // const Scalar wi = (isgn0(dEij) == isgn0(DepsDt0i + DepsDt0j) and isgn0(dEij) != 0 ?
              //                    DepsDt0i/(DepsDt0i + DepsDt0j) :
              //                    entropyWeighting(si, sj, duij));

              const Scalar wi = entropyWeighting(si, sj, duij);
              // const Scalar wi = standardWeighting(ui, uj, mi, mj, duij)
              // const Scalar wi = PoverRho2Weighting(Pi, rhoi, Pj, rhoj);
              // const Scalar wi = weighting(ui, uj, mi, mj, duij, dt);

              CHECK(wi >= 0.0 and wi <= 1.0);
              // CHECK2(fuzzyEqual(wi + entropyWeighting(sj, si, dEij/mj), 1.0, 1.0e-10),
              //        wi << " " << entropyWeighting(sj, si, dEij/mj) << " " << (wi + entropyWeighting(sj, si, dEij/mj)));
              DepsDti += wi*duij;
              DepsDtj += (1.0 - wi)*dEij/mj;
            }
          }
        }
      }

      // Add the self-contribution.
      if (pacci.size() == connectivityMap.numNeighborsForNode(nodeLists[nodeListi], i) + 1) {
        const Scalar duii = -vi12.dot(pacci.back());
        DepsDti += duii;
        ++offset(nodeListi, i);
      }

      // // Now we can update the energy.
      // CHECK2(offset(nodeListi, i) == pacci.size() or NodeListRegistrar<Dimension>::instance().domainDecompositionIndependent(),
      //        "Bad sizing : (" << nodeListi << " " << i << ") " << offset(nodeListi, i) << " " << pacci.size());
      // if (surface and surfacePoint(nodeListi, i) != 0) {
      //   eps(nodeListi, i) += DepsDt0(nodeListi, i)*multiplier;
      // } else {
      //      eps(nodeListi, i) += DepsDti*multiplier;
      // }

      eps(nodeListi, i) += DepsDti*multiplier;

    }
  }
}

//------------------------------------------------------------------------------
// Equivalence operator.
//------------------------------------------------------------------------------
template<typename Dimension>
bool
NonSymmetricSpecificThermalEnergyPolicy<Dimension>::
operator==(const UpdatePolicyBase<Dimension>& rhs) const {

  // We're only equal if the other guy is also an increment operator.
  const NonSymmetricSpecificThermalEnergyPolicy<Dimension>* rhsPtr = dynamic_cast<const NonSymmetricSpecificThermalEnergyPolicy<Dimension>*>(&rhs);
  if (rhsPtr == 0) {
    return false;
  } else {
    return true;
  }
}

}

