//---------------------------------Spheral++----------------------------------//
// ConstantZVelocityBoundary -- A boundary condition to enforce a constant 
// velocity on a given set of nodes.
//----------------------------------------------------------------------------//
#include "ConstantZVelocityBoundary.hh"
#include "Field/FieldList.hh"
#include "Field/Field.hh"
#include "Hydro/HydroFieldNames.hh"

#include "DBC.hh"
#include "cdebug.hh"

namespace Spheral {
namespace BoundarySpace {

using namespace std;

using NodeSpace::NodeList;
using FieldSpace::Field;
using FieldSpace::FieldList;
using FileIOSpace::FileIO;

//------------------------------------------------------------------------------
// Construct with the given set of nodes.
//------------------------------------------------------------------------------
template<typename Dimension>
ConstantZVelocityBoundary<Dimension>::
ConstantZVelocityBoundary(const NodeList<Dimension>& nodeList,
                          const vector<int>& nodeIndicies):
  ConstantVelocityBoundary<Dimension>(nodeList, nodeIndicies) {
  cdebug << "ConstantZVelocityBoundary::ConstantZVelocityBoundary" << this << endl;
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
template<typename Dimension>
ConstantZVelocityBoundary<Dimension>::~ConstantZVelocityBoundary() {
  cdebug << "ConstantZVelocityBoundary::~ConstantZVelocityBoundary() " << this << endl;
}

//------------------------------------------------------------------------------
// Apply the boundary condition to fields of different DataTypes.
//------------------------------------------------------------------------------
template<typename Dimension>
void
ConstantZVelocityBoundary<Dimension>::
enforceBoundary(Field<Dimension, typename Dimension::Vector>& field) const {
  cdebug << "ConstantZVelocityBoundary::enforceBoundary(VectorField) " << this << endl;

  REQUIRE(this->valid());

  // Is this field the velocity on the NodeList we're watching?
  if (field.nodeListPtr() == &(this->nodeList()) &&
      field.name() == HydroFieldNames::velocity) {

    // This is the velocity field, so enforce the boundary.
    int i = 0;
    const vector<int> nodeIDs = this->nodeIndicies();
    for (vector<int>::const_iterator itr = nodeIDs.begin();
         itr < nodeIDs.end();
         ++itr, ++i) {
      CHECK(*itr < field.numElements());
      CHECK(i < this->velocityCondition().size());
      field[*itr].z(this->velocityCondition()[i].z());
    }
  }
}

}
}

