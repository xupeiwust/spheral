include_directories(.)
set(DataOutput_sources
    RestartRegistrar.cc
    #RestartableObject.cc
    )

set(DataOutput_headers
    Restart.hh
    RestartHandle.hh
    RestartInline.hh
    RestartRegistrar.hh
    RestartRegistrarInline.hh
    registerWithRestart.hh
    )

spheral_add_cxx_library(DataOutput)

list(APPEND SPHERAL_PYTHON_DEPENDS Spheral_DataOutput)
set(SPHERAL_PYTHON_DEPENDS "${SPHERAL_PYTHON_DEPENDS}" PARENT_SCOPE)
