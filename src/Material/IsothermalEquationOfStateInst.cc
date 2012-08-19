//------------------------------------------------------------------------------
// Explicit instantiation.
//------------------------------------------------------------------------------
#include "IsothermalEquationOfState.cc"
#include "PhysicalConstants.hh"
#include "MKSUnits.hh"
#include "CGSUnits.hh"
#include "CosmologicalUnits.hh"
#include "SolarUnits.hh"

namespace Spheral {
  namespace Material {
    template class IsothermalEquationOfState<Dim<1>, PhysicalConstants<MKSUnits> >;
    template class IsothermalEquationOfState<Dim<2>, PhysicalConstants<MKSUnits> >;
    template class IsothermalEquationOfState<Dim<3>, PhysicalConstants<MKSUnits> >;
    template class IsothermalEquationOfState<Dim<1>, PhysicalConstants<CGSUnits> >;
    template class IsothermalEquationOfState<Dim<2>, PhysicalConstants<CGSUnits> >;
    template class IsothermalEquationOfState<Dim<3>, PhysicalConstants<CGSUnits> >;
    template class IsothermalEquationOfState<Dim<1>, PhysicalConstants<CosmologicalUnits> >;
    template class IsothermalEquationOfState<Dim<2>, PhysicalConstants<CosmologicalUnits> >;
    template class IsothermalEquationOfState<Dim<3>, PhysicalConstants<CosmologicalUnits> >;
    template class IsothermalEquationOfState<Dim<1>, PhysicalConstants<SolarUnits> >;
    template class IsothermalEquationOfState<Dim<2>, PhysicalConstants<SolarUnits> >;
    template class IsothermalEquationOfState<Dim<3>, PhysicalConstants<SolarUnits> >;
  }
}
