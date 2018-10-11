"""
Spheral SolidMaterial module.

Provides equations of state and material models for solids in Spheral.
"""

from PYB11Generator import *
from spheralDimensions import *
dims = spheralDimensions()

from SolidEquationOfState import *

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
includes = ['"Geometry/Dimension.hh"',
            '"SolidMaterial/SolidEquationOfState.hh"',
            '"SolidMaterial/LinearPolynomialEquationOfState.hh"',
            '"SolidMaterial/GruneisenEquationOfState.hh"',
            '"SolidMaterial/OsborneEquationOfState.hh"',
            '"SolidMaterial/TillotsonEquationOfState.hh"',
            '"SolidMaterial/MurnahanEquationOfState.hh"',
            '"SolidMaterial/StrengthModel.hh"',
            '"SolidMaterial/ConstantStrength.hh"',
            '"SolidMaterial/NullStrength.hh"',
            '"SolidMaterial/PolynomialFit.hh"',
            '"SolidMaterial/SteinbergGuinanStrength.hh"',
            '"SolidMaterial/SteinbergGuinanLundStrength.hh"',
            '"SolidMaterial/JohnsonCookStrength.hh"',
            '"SolidMaterial/CollinsStrength.hh"',
            '"SolidMaterial/PorousEquationOfState.hh"',
            '"SolidMaterial/PorousStrengthModel.hh"',
            '"SolidMaterial/StrainPorosity.hh"',
            '"SolidMaterial/PhysicsEvolvingMaterialLibrary.hh"',
            '"FileIO/FileIO.hh"',
            '<vector>',
            '<string>',
            '<iterator>']

#-------------------------------------------------------------------------------
# Namespaces
#-------------------------------------------------------------------------------
namespaces = ["Spheral"]

#-------------------------------------------------------------------------------
# Instantiate our types
#-------------------------------------------------------------------------------
for ndim in dims:
    exec('''
SolidEquationOfState%(ndim)id = PYB11TemplateClass(SolidEquationOfState, template_parameters="%(Dimension)s")
''' % {"ndim"      : ndim,
       "Dimension" : "Dim<" + str(ndim) + ">",
       "Vector"    : "Dim<" + str(ndim) + ">::Vector"})
