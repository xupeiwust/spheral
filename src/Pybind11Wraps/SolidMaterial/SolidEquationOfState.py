#-------------------------------------------------------------------------------
# SolidEquationOfState abstract class
#-------------------------------------------------------------------------------
from PYB11Generator import *
from EquationOfState import *

@PYB11template("Dimension")
@PYB11module("SpheralSolidMaterial")
class SolidEquationOfState(EquationOfState):
    "Abstract base for equations of state for solids"

    typedefs = """
    typedef typename %(Dimension)s::Scalar Scalar;
    typedef Field<%(Dimension)s, Scalar> ScalarField;
"""

    #...........................................................................
    # Constructors
    def pyinit(self,
               referenceDensity = "const double",
               etamin = "const double",
               etamax = "const double",
               constants = "const PhysicalConstants&",
               minimumPressure = "const double",
               maximumPressure = "const double",
               minPressureType = "const MaterialPressureMinType"):
        "Solid EOS base constructor"

    #...........................................................................
    # Virtual methods
    @PYB11virtual
    @PYB11const
    def valid(self):
        return "bool"

    #...........................................................................
    # Methods
    @PYB11const
    def boundedEta(self, rho="const double"):
        "Compute eta = rho/refrho, bounded to be in [etamin, etamax]."
        return "double"

    #...........................................................................
    # Properties
    referenceDensity = PYB11property("double", "referenceDensity", "referenceDensity")
    etamin = PYB11property("double", "etamin", "etamin")
    etamax = PYB11property("double", "etamax", "etamax")
