//------------------------------------------------------------------------------
// Compute the CRKSPH mass density summation.
//------------------------------------------------------------------------------
#include "computeCRKSPHSumMassDensity.hh"
#include "computeCRKSPHMoments.hh"
#include "computeCRKSPHCorrections.hh"
#include "CRKSPHCorrectionParams.hh"
#include "NodeList/NodeList.hh"
#include "Hydro/HydroFieldNames.hh"

using std::vector;
using std::min;
using std::max;
using std::abs;
using std::vector;

namespace Spheral {

template<typename Dimension>
void
computeCRKSPHSumMassDensity(const ConnectivityMap<Dimension>& connectivityMap,
                            const TableKernel<Dimension>& W,
                            const FieldList<Dimension, typename Dimension::Vector>& position,
                            const FieldList<Dimension, typename Dimension::Scalar>& mass,
                            const FieldList<Dimension, typename Dimension::Scalar>& vol,
                            const FieldList<Dimension, typename Dimension::SymTensor>& H,
                            FieldList<Dimension, typename Dimension::Scalar>& massDensity) {

  // Pre-conditions.
  const size_t numNodeLists = massDensity.size();
  REQUIRE(position.size() == numNodeLists);
  REQUIRE(mass.size() == numNodeLists);
  REQUIRE(vol.size() == numNodeLists);
  REQUIRE(H.size() == numNodeLists);

  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;
  typedef typename Dimension::SymTensor SymTensor;
  typedef typename Dimension::ThirdRankTensor ThirdRankTensor;
  typedef typename Dimension::FourthRankTensor FourthRankTensor;
  typedef typename Dimension::FifthRankTensor FifthRankTensor;

  // Some scratch variables.
  Scalar Wi, Wj;
  Vector rij, etai, etaj;
  Vector Bi = Vector::zero, Bj = Vector::zero;
  Tensor Ci = Tensor::zero, Cj = Tensor::zero;

  FieldList<Dimension, Scalar> wsum(FieldStorageType::CopyFields), vol1(FieldStorageType::CopyFields);
  for (size_t nodeListi = 0; nodeListi != numNodeLists; ++nodeListi) {
    wsum.appendNewField("weight sum", position[nodeListi]->nodeList(), 0.0);
    vol1.appendNewField("sampled volume", position[nodeListi]->nodeList(), 0.0);
  }

  massDensity = 0.0;
  for (size_t nodeListi = 0; nodeListi != numNodeLists; ++nodeListi) {
    const auto& nodeList = dynamic_cast<const FluidNodeList<Dimension>&>(massDensity[nodeListi]->nodeList());
    const auto  rhoMin = nodeList.rhoMin();
    const auto  rhoMax = nodeList.rhoMax();

    // Iterate over the nodes in this node list.
    for (auto iItr = connectivityMap.begin(nodeListi);
         iItr != connectivityMap.end(nodeListi);
         ++iItr) {
      const auto i = *iItr;

      // Get the state for node i.
      const auto& ri = position(nodeListi, i);
      const auto  mi = mass(nodeListi, i);
      const auto  Vi = vol(nodeListi, i);
      const auto  rhoi = massDensity(nodeListi, i);
      const auto& Hi = H(nodeListi, i);
      const auto  Hdeti = Hi.Determinant();

      // Loop over the neighbors in just point i's NodeList.
      const auto& fullConnectivity = connectivityMap.connectivityForNode(nodeListi, i);
      const auto& connectivity = fullConnectivity[nodeListi];
      const auto  firstGhostNodej = massDensity[nodeListi]->nodeList().firstGhostNode();
      for (auto jItr = connectivity.begin();
           jItr != connectivity.end();
           ++jItr) {
        const auto j = *jItr;

        // Check if this node pair has already been calculated.
        if (connectivityMap.calculatePairInteraction(nodeListi, i, 
                                                     nodeListi, j,
                                                     firstGhostNodej)) {
          const auto& rj = position(nodeListi, j);
          const auto  mj = mass(nodeListi, j);
          const auto  Vj = vol(nodeListi, j);
          const auto  rhoj = massDensity(nodeListi, j);
          const auto& Hj = H(nodeListi, j);
          const auto  Hdetj = Hj.Determinant();

          // Kernel weighting and gradient.
          rij = ri - rj;
          etai = Hi*rij;
          etaj = Hj*rij;
          Wi = W.kernelValue(etai.magnitude(), Hdeti);
          Wj = W.kernelValue(etaj.magnitude(), Hdetj);

          // Sum the pair-wise contributions.
          wsum(nodeListi, i) += Vj*Wj;
          wsum(nodeListi, j) += Vi*Wi;
          massDensity(nodeListi, i) += mj * Vj*Wj;
          massDensity(nodeListi, j) += mi * Vi*Wi;
          vol1(nodeListi, i) += Vj * Vj*Wj;
          vol1(nodeListi, j) += Vi * Vi*Wi;
        }
      }
  
      // Finalize the density and volume for node i.
      const Scalar W0 = W.kernelValue(0.0, Hdeti);
      wsum(nodeListi, i) += Vi*W0;
      CHECK(wsum(nodeListi, i) > 0.0);
      vol1(nodeListi, i) = (vol1(nodeListi, i) + Vi*Vi*W0)/wsum(nodeListi, i);
      massDensity(nodeListi, i) = max(max(rhoMin, 0.1*mi*H(nodeListi, i).Determinant()),
                                      min(rhoMax,
                                          (massDensity(nodeListi, i) + mi*Vi*W0)/
                                          (wsum(nodeListi, i)*vol1(nodeListi, i))));
      ENSURE(vol1(nodeListi, i) > 0.0);
      ENSURE(massDensity(nodeListi, i) > 0.0);
    }
  }
}

}
