#-------------------------------------------------------------------------------
# StateBase
#-------------------------------------------------------------------------------
from PYB11Generator import *

@PYB11template("Dimension")
class StateBase:

    typedefs = """
    typedef typename %(Dimension)s::Scalar Scalar;
    typedef typename %(Dimension)s::Vector Vector;
    typedef typename %(Dimension)s::Tensor Tensor;
    typedef typename %(Dimension)s::SymTensor SymTensor;
    typedef typename StateBase<%(Dimension)s>::KeyType KeyType;
    typedef typename StateBase<%(Dimension)s>::FieldName FieldName;
    typedef typename StateBase<%(Dimension)s>::MeshPtr MeshPtr;
"""

    #...........................................................................
    # Constructors
    def pyinit(self):
        "Default constructor"

    #...........................................................................
    # Virtual methods
    @PYB11virtual
    def enroll(self, field="FieldBase<%(Dimension)s>&"):
        "Enroll a field to track"
        return "void"

    @PYB11virtual
    @PYB11pycppname("enroll")
    def enroll_sharedptr(self, field="std::shared_ptr<FieldBase<%(Dimension)s>>&"):
        "Enroll a shared_ptr<Field> to track"
        return "void"

    @PYB11virtual
    @PYB11pycppname("enroll")
    def enroll_fieldlist(self, fieldList="FieldListBase<%(Dimension)s>&"):
        "Enroll a FieldList to track"
        return "void"

    @PYB11virtual
    def copyState(self):
        "Make an internal copy of all state referenced by this StateBase."
        return "void"

    #...........................................................................
    # Operators
    def __eq__(self):
        return

    #...........................................................................
    # Methods
    @PYB11const
    def registered(self, key="const KeyType&"):
        "Test if there is state registered corresponding to the Key"
        return "bool"

    @PYB11const
    @PYB11pycppname("registered")
    def registered1(self, field="const FieldBase<%(Dimension)s>&"):
        "Test if the specified Field is registered"
        return "bool"

    @PYB11const
    @PYB11pycppname("registered")
    def registered2(self, fieldList="const FieldListBase<%(Dimension)s>&"):
        "Test if the specified FieldList is registered"
        return "bool"

    @PYB11const
    @PYB11pycppname("registered")
    def fieldNameRegistered(self, fieldName="const std::string&"):
        "Test if the a Field with the given name is registered"
        return "bool"

    def enrollMesh(self, meshPtr="MeshPtr"):
        "Enroll a mesh for tracking"
        return "void"

    # @PYB11pycppname("enroll")
    # def enroll_vec(self,
    #                key = "const std::string&",
    #                vec = "std::vector<Scalar>&"):
    #     "Enroll a vector<Scalar> using the given key"
    #     return "void"

    # @PYB11returnpolicy("reference_internal")
    # def array(self, key="const std::string&"):
    #     "Get the vector<double> associated with the given key"
    #     return "std::vector<Scalar>&"

    @PYB11const
    def keys(self):
        "The set of keys for state in the StateBase"
        return "std::vector<KeyType>"

    @PYB11const
    def fieldKeys(self):
        "The set of Field names for the state in the StateBase"
        return "std::vector<FieldName>"

    def enrollConnectivityMap(self,
                              connectivityMapPtr = "std::shared_ptr<ConnectivityMap<%(Dimension)s>>"):
        "Enroll the ConnectivityMap"
        return "void"

    @PYB11returnpolicy("reference_internal")
    @PYB11const
    def connectivityMap(self):
        "Get the ConnectivityMap"
        return "const ConnectivityMap<%(Dimension)s>&"

    @PYB11const
    def meshRegistered(self):
        "Test if a mesh is registered"
        return "bool"

    @PYB11returnpolicy("reference_internal")
    @PYB11const
    def mesh(self):
        "Get the currently registered Mesh"
        return "const Mesh<%(Dimension)s>&"

    def assign(self, rhs="const StateBase<%(Dimension)s>&"):
        "Set this StateBase's state equal to the other"
        return "void"

    @PYB11static
    def key(self, field="const FieldBase<%(Dimension)s>&"):
        "Construct a key for the given Field"
        return "KeyType"

    @PYB11static
    @PYB11pycppname("key")
    def key1(self, fieldList="const FieldListBase<%(Dimension)s>&"):
        "Construct a key for the given FieldList"
        return "KeyType"

    @PYB11static
    def buildFieldKey(self,
                      fieldName = "const KeyType&",
                      nodeListName = "const KeyType&"):
        "Hash a key based on the (fieldname, node list name)"
        return "KeyType"

    @PYB11static
    def splitFieldKey(self,
                      key = "const KeyType&",
                      fieldName = "KeyType&",
                      nodeListName = "KeyType&"):
        "Split the hashed key back into (field name, node list name) info"
        return "void"

    #...........................................................................
    # Template methods
    @PYB11template("Value")
    @PYB11returnpolicy("reference_internal")
    @PYB11const
    def field(self,
              key = "const KeyType&",
              dummy = ("const %(Value)s&", "%(Value)s()")):
        "Return the %(Value)s field based on the key"
        return "Field<%(Dimension)s, %(Value)s>&"

    @PYB11template("Value")
    @PYB11const
    def fields(self,
               name = "const KeyType&",
               dummy = ("const %(Value)s&", "%(Value)s()")):
        "Return the %(Value)s FieldList based on the name"
        return "FieldList<%(Dimension)s, %(Value)s>"

    @PYB11template("Value")
    @PYB11const
    def allFields(self,
                  dummy = ("const %(Value)s&", "%(Value)s()")):
        "Return a set of all the %(Value)s Fields in the StateBase"
        return "std::vector<Field<%(Dimension)s, %(Value)s>*>"

    intField = PYB11TemplateMember(field, "int")
    scalarField = PYB11TemplateMember(field, "double")
    vectorField = PYB11TemplateMember(field, "Vector")
    tensorField = PYB11TemplateMember(field, "Tensor")
    symTensorField = PYB11TemplateMember(field, "SymTensor")

    intFields = PYB11TemplateMember(fields, "int")
    scalarFields = PYB11TemplateMember(fields, "double")
    vectorFields = PYB11TemplateMember(fields, "Vector")
    tensorFields = PYB11TemplateMember(fields, "Tensor")
    symTensorFields = PYB11TemplateMember(fields, "SymTensor")

    allIntFields = PYB11TemplateMember(allFields, "int")
    allScalarFields = PYB11TemplateMember(allFields, "double")
    allVectorFields = PYB11TemplateMember(allFields, "Vector")
    allTensorFields = PYB11TemplateMember(allFields, "Tensor")
    allSymTensorFields = PYB11TemplateMember(allFields, "SymTensor")
