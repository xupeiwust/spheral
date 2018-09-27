"""
Spheral DataBase module.

Provides higher level interface for State and DataBase objects.
"""

from PYB11Generator import *
from spheralDimensions import *
dims = spheralDimensions()

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
includes = ['"Geometry/Dimension.hh"',
            '"DataBase/DataBase.hh"',
            '"DataBase/StateBase.hh"',
            '"DataBase/State.hh"',
            '"DataBase/StateDerivatives.hh"',
            '"Field/Field.hh"',
            '"Neighbor/ConnectivityMap.hh"',
            '"Physics/Physics.hh"',
            '<vector>',
            '<string>']

#-------------------------------------------------------------------------------
# Namespaces
#-------------------------------------------------------------------------------
namespaces = ["Spheral"]

#-------------------------------------------------------------------------------
# Instantiate our types
#-------------------------------------------------------------------------------
from StateBase import *
from State import *

for ndim in dims:
    exec('''
StateBase%(ndim)id = PYB11TemplateClass(StateBase, template_parameters="Dim<%(ndim)i>")
State%(ndim)id = PYB11TemplateClass(State, template_parameters="Dim<%(ndim)i>")
''' % {"ndim" : ndim})
