#-------------------------------------------------------------------------------
# DataBase
#-------------------------------------------------------------------------------
from PYB11Generator import *
from DataBase import *

@PYB11template("Dimension")
class DataBase:

    typedefs = """
    typedef typename %(Dimension)s::Scalar Scalar;
    typedef typename %(Dimension)s::Vector Vector;
    typedef typename %(Dimension)s::Tensor Tensor;
    typedef typename %(Dimension)s::SymTensor SymTensor;
    typedef typename %(Dimension)s::ThirdRankTensor ThirdRankTensor;
    typedef typename %(Dimension)s::FourthRankTensor FourthRankTensor;
    typedef typename %(Dimension)s::FifthRankTensor FifthRankTensor;
    typedef typename %(Dimension)s::FacetedVolume FacetedVolume;
    typedef typename DataBase<%(Dimension)s>::ConnectivityMapPtr ConnectivityMapPtr;
"""

    #...........................................................................
    # Constructors
    def pyinit(self):
        "Default constructor"

    #...........................................................................
    # Methods
    @PYB11const
    def reinitializeNeighbors(self):
        "Optimize all Neighbor objects for the current state."
        return "void"

    @PYB11const
    def updateConnectivityMap(self, computeGhostConnectivity="const bool"):
        "Update the internal connectivity map."
        return "void"

    @PYB11const
    def patchConnectivityMap(self,
                             flags = "const FieldList<%(Dimension)s, int>&",
                             old2new = "const FieldList<%(Dimension)s, int>&"):
        "Update the internal connectivity map."
        return "void"

    @PYB11returnpolicy("reference_internal")
    @PYB11const
    def connectivityMap(self, computeGhostConnectivity=("const bool", "false")):
        "Get the connectivity map, optionally including ghost connectivity"
        return "const ConnectivityMap<%(Dimension)s>&"

    @PYB11const
    def connectivityMapPtr(self, computeGhostConnectivity=("const bool", "false")):
        "Get the connectivity map as a std::shared_ptr, optionally including ghost connectivity"
        return "ConnectivityMapPtr"

    def appendNodeList(self, nodeList="SolidNodeList<%(Dimension)s>&"):
        "Add a SolidNodeList"
        return "void"

    @PYB11pycppname("appendNodeList")
    def appendNodeList1(self, nodeList="FluidNodeList<%(Dimension)s>&"):
        "Add a FluidNodeList"
        return "void"

    @PYB11pycppname("appendNodeList")
    def appendNodeList2(self, nodeList="NodeList<%(Dimension)s>&"):
        "Add a NodeList"
        return "void"

    def deleteNodeList(self, nodeList="SolidNodeList<%(Dimension)s>&"):
        "Remove a SolidNodeList"
        return "void"

    @PYB11pycppname("deleteNodeList")
    def deleteNodeList1(self, nodeList="FluidNodeList<%(Dimension)s>&"):
        "Remove a FluidNodeList"
        return "void"

    @PYB11pycppname("deleteNodeList")
    def deleteNodeList2(self, nodeList="NodeList<%(Dimension)s>&"):
        "Remove a NodeList"
        return "void"

    @PYB11const
    def haveNodeList(self, nodeList="NodeList<%(Dimension)s>&"):
        "Check if a NodeList is in the DataBase"
        return "bool"

    @PYB11const
    def setMasterNodeLists(self,
                           position = "const Vector&",
                           H = "const SymTensor&",
                           masterLists = "std::vector<std::vector<int>>&",
                           coarseNeighbors = "std::vector<std::vector<int>>&"):
        "Set the master/coarse neighbor lists for all NodeLists"
        return "void"

    @PYB11const
    def setMasterFluidNodeLists(self,
                                position = "const Vector&",
                                H = "const SymTensor&",
                                masterLists = "std::vector<std::vector<int>>&",
                                coarseNeighbors = "std::vector<std::vector<int>>&"):
        "Set the master/coarse neighbor lists for all NodeLists"
        return "void"

    @PYB11const
    def setRefineNodeLists(self,
                           position = "const Vector&",
                           H = "const SymTensor&",
                           coarseNeighbors = "const std::vector<std::vector<int>>&",
                           refineNeighbors = "std::vector<std::vector<int>>&"):
        "Set the refine neighbor lists for all NodeLists"
        return "void"

    @PYB11const
    def setRefineFluidNodeLists(self,
                                position = "const Vector&",
                                H = "const SymTensor&",
                                coarseNeighbors = "const std::vector<std::vector<int>>&",
                                refineNeighbors = "std::vector<std::vector<int>>&"):
        "Set the refine neighbor lists for all FluidNodeLists"
        return "void"

    @PYB11const
    def globalHinverse(self, result="FieldList<%(Dimension)s, SymTensor>&"):
        return "void"

    @PYB11const
    def fluidHinverse(self, result="FieldList<%(Dimension)s, SymTensor>&"):
        return "void"

    @PYB11const
    def fluidPressure(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidTemperature(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidSoundSpeed(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidVolume(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidGamma(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidEntropy(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def fluidLinearMomentum(self, result="FieldList<%(Dimension)s, Vector>&"):
        return "void"

    @PYB11const
    def fluidTotalEnergy(self, result="FieldList<%(Dimension)s, Scalar>&"):
        return "void"

    @PYB11const
    def boundingBox(self,
                    xmin = "Vector&",
                    xmax = "Vector&",
                    ghost = ("const bool", "true")):
        "Compute coordinates bounding all nodes in the DataBase."
        return "void"

    @PYB11const
    def boundingBox(self,
                    xmin = "Vector&",
                    xmax = "Vector&",
                    mask = "const FieldList<%(Dimension)s, int>&",
                    ghost = ("const bool", "true")):
        "Compute coordinates bounding all nodes in the DataBase."
        return "void"

    @PYB11const
    def localSamplingBoundingVolume(self,
                                    centroid = "Vector&",
                                    radiusNodes = "double&",
                                    radiusSample = "double&",
                                    xminNodes = "Vector&",
                                    xmaxNodes = "Vector&",
                                    xminSample = "Vector&",
                                    xmaxSample = "Vector&"):
        "Return the local max sampling extents."
        return "void"

    @PYB11const
    def globalSamplingBoundingVolume(self,
                                     centroid = "Vector&",
                                     radiusNodes = "double&",
                                     radiusSample = "double&",
                                     xminNodes = "Vector&",
                                     xmaxNodes = "Vector&",
                                     xminSample = "Vector&",
                                     xmaxSample = "Vector&"):
        "Return the global max sampling extents."
        return "void"

    @PYB11const
    def localSamplingBoundingBoxes(self,
                                   xminima = "std::vector<Vector>&",
                                   xmaxima = "std::vector<Vector>&"):
        "Return the local min and max sampling extents for groupings of connected nodes."
        return "void"

    @PYB11const
    def globalSamplingBoundingBoxes(self,
                                    xminima = "std::vector<Vector>&",
                                    xmaxima = "std::vector<Vector>&"):
        "Return the global min and max sampling extents for groupings of connected nodes."
        return "void"

    @PYB11const
    def valid(self):
        "Provide a method to determine if the DataBase is in a minimally defined valid state."
        return "bool"

    #...........................................................................
    # Template methods
    @PYB11template("DataType")
    @PYB11const
    def newGlobalFieldList(self,
                           value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                           name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"')):
        "Construct a new FieldList<%(DataType)s> for all NodeLists in DataBase"
        return "FieldList<%(Dimension)s, %(DataType)s>"

    @PYB11template("DataType")
    @PYB11const
    def newFluidFieldList(self,
                          value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                          name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"')):
        "Construct a new FieldList<%(DataType)s> for all FluidNodeLists in DataBase"
        return "FieldList<%(Dimension)s, %(DataType)s>"

    @PYB11template("DataType")
    @PYB11const
    def newSolidFieldList(self,
                          value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                          name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"')):
        "Construct a new FieldList<%(DataType)s> for all SolidNodeLists in DataBase"
        return "FieldList<%(Dimension)s, %(DataType)s>"

    @PYB11template("DataType")
    @PYB11const
    def resizeGlobalFieldList(self,
                              fieldList = "FieldList<%(Dimension)s, %(DataType)s>&",
                              value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                              name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"'),
                              resetValues = ("const bool", "true")):
        """Resize a FieldList to the number of NodeLists.
Optionally we can also set all elements in the FieldList to the specified value.
Note that if the FieldList is resized it is reconstructed from scratch, so all elements
will get the new value regardless of resetValues."""
        return "void"

    @PYB11template("DataType")
    @PYB11const
    def resizeFluidFieldList(self,
                             fieldList = "FieldList<%(Dimension)s, %(DataType)s>&",
                             value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                             name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"'),
                             resetValues = ("const bool", "true")):
        """Resize a FieldList to the number of FluidNodeLists.
Optionally we can also set all elements in the FieldList to the specified value.
Note that if the FieldList is resized it is reconstructed from scratch, so all elements
will get the new value regardless of resetValues."""
        return "void"

    @PYB11template("DataType")
    @PYB11const
    def resizeSolidFieldList(self,
                             fieldList = "FieldList<%(Dimension)s, %(DataType)s>&",
                             value = ("const %(DataType)s", "DataTypeTraits<%(DataType)s>::zero()"),
                             name = ("const Field<%(Dimension)s, %(DataType)s>::FieldName", '"Unnamed Field"'),
                             resetValues = ("const bool", "true")):
        """Resize a FieldList to the number of SolidNodeLists.
Optionally we can also set all elements in the FieldList to the specified value.
Note that if the FieldList is resized it is reconstructed from scratch, so all elements
will get the new value regardless of resetValues."""
        return "void"

    newGlobalIntFieldList              = PYB11TemplateMethod(newGlobalFieldList, template_parameters="int")
    newGlobalScalarFieldList           = PYB11TemplateMethod(newGlobalFieldList, template_parameters="double")
    newGlobalVectorFieldList           = PYB11TemplateMethod(newGlobalFieldList, template_parameters="Vector")
    newGlobalTensorFieldList           = PYB11TemplateMethod(newGlobalFieldList, template_parameters="Tensor")
    newGlobalSymTensorFieldList        = PYB11TemplateMethod(newGlobalFieldList, template_parameters="SymTensor")
    newGlobalThirdRankTensorFieldList  = PYB11TemplateMethod(newGlobalFieldList, template_parameters="ThirdRankTensor")
    newGlobalFourthRankTensorFieldList = PYB11TemplateMethod(newGlobalFieldList, template_parameters="FourthRankTensor")
    newGlobalFifthRankTensorFieldList  = PYB11TemplateMethod(newGlobalFieldList, template_parameters="FifthRankTensor")
    newGlobalvector_of_doubleFieldList = PYB11TemplateMethod(newGlobalFieldList, template_parameters="std::vector<double>")
    newGlobalvector_of_VectorFieldList = PYB11TemplateMethod(newGlobalFieldList, template_parameters="std::vector<Vector>")

    newFluidIntFieldList              = PYB11TemplateMethod(newFluidFieldList, template_parameters="int")
    newFluidScalarFieldList           = PYB11TemplateMethod(newFluidFieldList, template_parameters="double")
    newFluidVectorFieldList           = PYB11TemplateMethod(newFluidFieldList, template_parameters="Vector")
    newFluidTensorFieldList           = PYB11TemplateMethod(newFluidFieldList, template_parameters="Tensor")
    newFluidSymTensorFieldList        = PYB11TemplateMethod(newFluidFieldList, template_parameters="SymTensor")
    newFluidThirdRankTensorFieldList  = PYB11TemplateMethod(newFluidFieldList, template_parameters="ThirdRankTensor")
    newFluidFourthRankTensorFieldList = PYB11TemplateMethod(newFluidFieldList, template_parameters="FourthRankTensor")
    newFluidFifthRankTensorFieldList  = PYB11TemplateMethod(newFluidFieldList, template_parameters="FifthRankTensor")
    newFluidvector_of_doubleFieldList = PYB11TemplateMethod(newFluidFieldList, template_parameters="std::vector<double>")
    newFluidvector_of_VectorFieldList = PYB11TemplateMethod(newFluidFieldList, template_parameters="std::vector<Vector>")

    newSolidIntFieldList              = PYB11TemplateMethod(newSolidFieldList, template_parameters="int")
    newSolidScalarFieldList           = PYB11TemplateMethod(newSolidFieldList, template_parameters="double")
    newSolidVectorFieldList           = PYB11TemplateMethod(newSolidFieldList, template_parameters="Vector")
    newSolidTensorFieldList           = PYB11TemplateMethod(newSolidFieldList, template_parameters="Tensor")
    newSolidSymTensorFieldList        = PYB11TemplateMethod(newSolidFieldList, template_parameters="SymTensor")
    newSolidThirdRankTensorFieldList  = PYB11TemplateMethod(newSolidFieldList, template_parameters="ThirdRankTensor")
    newSolidFourthRankTensorFieldList = PYB11TemplateMethod(newSolidFieldList, template_parameters="FourthRankTensor")
    newSolidFifthRankTensorFieldList  = PYB11TemplateMethod(newSolidFieldList, template_parameters="FifthRankTensor")
    newSolidvector_of_doubleFieldList = PYB11TemplateMethod(newSolidFieldList, template_parameters="std::vector<double>")
    newSolidvector_of_VectorFieldList = PYB11TemplateMethod(newSolidFieldList, template_parameters="std::vector<Vector>")

    resizeGlobalIntFieldList              = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="int")
    resizeGlobalScalarFieldList           = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="double")
    resizeGlobalVectorFieldList           = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="Vector")
    resizeGlobalTensorFieldList           = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="Tensor")
    resizeGlobalSymTensorFieldList        = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="SymTensor")
    resizeGlobalThirdRankTensorFieldList  = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="ThirdRankTensor")
    resizeGlobalFourthRankTensorFieldList = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="FourthRankTensor")
    resizeGlobalFifthRankTensorFieldList  = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="FifthRankTensor")
    resizeGlobalvector_of_doubleFieldList = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="std::vector<double>")
    resizeGlobalvector_of_VectorFieldList = PYB11TemplateMethod(resizeGlobalFieldList, template_parameters="std::vector<Vector>")

    resizeFluidIntFieldList              = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="int")
    resizeFluidScalarFieldList           = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="double")
    resizeFluidVectorFieldList           = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="Vector")
    resizeFluidTensorFieldList           = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="Tensor")
    resizeFluidSymTensorFieldList        = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="SymTensor")
    resizeFluidThirdRankTensorFieldList  = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="ThirdRankTensor")
    resizeFluidFourthRankTensorFieldList = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="FourthRankTensor")
    resizeFluidFifthRankTensorFieldList  = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="FifthRankTensor")
    resizeFluidvector_of_doubleFieldList = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="std::vector<double>")
    resizeFluidvector_of_VectorFieldList = PYB11TemplateMethod(resizeFluidFieldList, template_parameters="std::vector<Vector>")

    resizeSolidIntFieldList              = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="int")
    resizeSolidScalarFieldList           = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="double")
    resizeSolidVectorFieldList           = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="Vector")
    resizeSolidTensorFieldList           = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="Tensor")
    resizeSolidSymTensorFieldList        = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="SymTensor")
    resizeSolidThirdRankTensorFieldList  = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="ThirdRankTensor")
    resizeSolidFourthRankTensorFieldList = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="FourthRankTensor")
    resizeSolidFifthRankTensorFieldList  = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="FifthRankTensor")
    resizeSolidvector_of_doubleFieldList = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="std::vector<double>")
    resizeSolidvector_of_VectorFieldList = PYB11TemplateMethod(resizeSolidFieldList, template_parameters="std::vector<Vector>")

    #...........................................................................
    # Attributes
    nDim = PYB11readonly(static=True)
    isRZ = PYB11readonly(static=True)

    #...........................................................................
    # Properties
    numNodeLists = PYB11property("int", "numNodeLists", doc="Number of NodeLists in DataBase")
    numFluidNodeLists = PYB11property("int", "numFluidNodeLists", doc="Number of FluidNodeLists in DataBase")
    numSolidNodeLists = PYB11property("int", "numSolidNodeLists", doc="Number of SolidNodeLists in DataBase")

    numNodes = PYB11property("int", "numNodes", doc="Number of nodes in all NodeLists in DataBase")
    numInternalNodes = PYB11property("int", "numInternalNodes", doc="Number of internal nodes in all NodeLists in DataBase")
    numGhostNodes = PYB11property("int", "numGhostNodes", doc="Number of ghost nodes in all NodeLists in DataBase")

    globalNumNodes = PYB11property("int", "globalNumNodes", doc="Number of nodes in all NodeLists in DataBase across all processors")
    globalNumInternalNodes = PYB11property("int", "globalNumInternalNodes", doc="Number of internal nodes in all NodeLists in DataBase across all processors")
    globalNumGhostNodes = PYB11property("int", "globalNumGhostNodes", doc="Number of ghost nodes in all NodeLists in DataBase across all processors")

    nodeListPtrs = PYB11property("const std::vector<NodeList<%(Dimension)s>*>&", "nodeListPtrs", doc="The set of NodeLists in the DataBase")
    fluidNodeListPtrs = PYB11property("const std::vector<FluidNodeList<%(Dimension)s>*>&", "fluidNodeListPtrs", doc="The set of FluidNodeLists in the DataBase")
    solidNodeListPtrs = PYB11property("const std::vector<SolidNodeList<%(Dimension)s>*>&", "solidNodeListPtrs", doc="The set of SolidNodeLists in the DataBase")

    maxKernelExtent = PYB11property("double")

    globalMass = PYB11property("FieldList<%(Dimension)s, Scalar>")
    globalPosition = PYB11property("FieldList<%(Dimension)s, Vector>")
    globalVelocity = PYB11property("FieldList<%(Dimension)s, Vector>")
    globalHfield = PYB11property("FieldList<%(Dimension)s, SymTensor>")
    globalWork = PYB11property("FieldList<%(Dimension)s, Scalar>")

    fluidMass = PYB11property("FieldList<%(Dimension)s, Scalar>")
    fluidPosition = PYB11property("FieldList<%(Dimension)s, Vector>")
    fluidVelocity = PYB11property("FieldList<%(Dimension)s, Vector>")
    fluidMassDensity = PYB11property("FieldList<%(Dimension)s, Scalar>")
    fluidSpecificThermalEnergy = PYB11property("FieldList<%(Dimension)s, Scalar>")
    fluidHfield = PYB11property("FieldList<%(Dimension)s, SymTensor>")
    fluidWork = PYB11property("FieldList<%(Dimension)s, Scalar>")

    solidDeviatoricStress = PYB11property("FieldList<%(Dimension)s, SymTensor>")
    solidPlasticStrain = PYB11property("FieldList<%(Dimension)s, Scalar>")
    solidPlasticStrainRate = PYB11property("FieldList<%(Dimension)s, Scalar>")
    solidDamage = PYB11property("FieldList<%(Dimension)s, SymTensor>")
    solidEffectiveDamage = PYB11property("FieldList<%(Dimension)s, SymTensor>")
    solidDamageGradient = PYB11property("FieldList<%(Dimension)s, Vector>")
    solidFragmentIDs = PYB11property("FieldList<%(Dimension)s, int>")

    globalNodeExtent = PYB11property("FieldList<%(Dimension)s, Vector>")
    fluidNodeExtent = PYB11property("FieldList<%(Dimension)s, Vector>")
