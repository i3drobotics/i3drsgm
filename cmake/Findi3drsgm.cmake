cmake_minimum_required(VERSION 2.6)

find_package(PhobosIntegration REQUIRED)

if (NOT i3drsgm_DIR)
    message( FATAL_ERROR "MUST define i3drsgm_DIR" )
endif()

find_path(I3DRSGM_INCLUDE_DIR
    NAMES I3DRSGM/i3drsgm.h
    HINTS ${i3drsgm_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    DOC "The I3DRSGM include directory"
)

if (NOT I3DRSGM_INCLUDE_DIR)
    message(WARNING "include directory not found")
endif()

find_library(I3DRSGM_LIBRARY 
    NAMES I3DRSGM
    PATHS ${i3drsgm_DIR}/lib
    NO_DEFAULT_PATH
    DOC "The I3DRSGM library"
)

if (NOT I3DRSGM_LIBRARY)
    message(WARNING "library directory not found")
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LOGGING_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(I3DRSGM DEFAULT_MSG I3DRSGM_INCLUDE_DIR I3DRSGM_LIBRARY)

if (I3DRSGM_FOUND)
    set(i3drsgm_LIBRARIES ${I3DRSGM_LIBRARY} ${PhobosIntegration_LIBRARIES} )
    set(i3drsgm_INCLUDE_DIRS ${I3DRSGM_INCLUDE_DIR} ${PhobosIntegration_INCLUDE_DIRS} )
    set(i3drsgm_DEFINITIONS )
else(I3DRSGM_FOUND)
    message(WARNING "I3DRSGM not found")
endif()

# Tell cmake GUIs to ignore the "local" variables.
mark_as_advanced(i3drsgm_DIR I3DRSGM_LIBRARY I3DRSGM_LIBRARY) 