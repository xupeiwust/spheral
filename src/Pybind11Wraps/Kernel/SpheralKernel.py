"""
Spheral Kernel module.

Provide the standard SPH/CRK interpolation kernels.
"""

from PYB11Generator import *
from spheralDimensions import *
dims = spheralDimensions()

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------
includes = ['"Geometry/Dimension.hh"',
            '"Kernel/BSplineKernel.hh"',
            '"Kernel/W4SplineKernel.hh"',
            '"Kernel/GaussianKernel.hh"',
            '"Kernel/SuperGaussianKernel.hh"',
            '"Kernel/PiGaussianKernel.hh"',
            '"Kernel/HatKernel.hh"',
            '"Kernel/SincKernel.hh"',
            '"Kernel/NSincPolynomialKernel.hh"',
            '"Kernel/NBSplineKernel.hh"',
            '"Kernel/QuarticSplineKernel.hh"',
            '"Kernel/QuinticSplineKernel.hh"',
            '"Kernel/TableKernel.hh"',
            '"Kernel/WendlandC2Kernel.hh"',
            '"Kernel/WendlandC4Kernel.hh"',
            '"Kernel/WendlandC6Kernel.hh"',
            '"Kernel/ExpInvKernel.hh"',
            '<vector>',
            '<string>']

#-------------------------------------------------------------------------------
# Namespaces
#-------------------------------------------------------------------------------
namespaces = ["Spheral"]

#-------------------------------------------------------------------------------
# Instantiate our types
#-------------------------------------------------------------------------------
from Kernel import *

for ndim in dims:
BSplineKernel1d = PYB11TemplateClass(Kernel,
                                     template_parameters = ("Dim<1>", "BSplineKernel<Dim<1>>"),
                                     pyname = "BSplineKernel1d",
                                     cppname = "BSplineKernel<Dim<1>>")
