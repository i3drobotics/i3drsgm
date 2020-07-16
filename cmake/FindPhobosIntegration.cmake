cmake_minimum_required(VERSION 2.6)

if (NOT PhobosIntegration_DIR)
    message( FATAL_ERROR "MUST define PhobosIntegration_DIR" )
endif()

find_path(PHOBOSINTEGRATION_INCLUDE_DIR
    NAMES PhobosIntegration/PhobosIntegration.hpp
    HINTS ${PhobosIntegration_DIR}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    DOC "The Phobos include directory"
)

if (NOT PHOBOSINTEGRATION_INCLUDE_DIR)
    message(WARNING "include directory not found")
endif()

find_library(PHOBOSINTEGRATION_LIBRARY 
    NAMES PhobosIntegration
    PATHS ${PhobosIntegration_DIR}/lib/PhobosIntegration
    NO_DEFAULT_PATH
    DOC "The Phobos library"
)

if (NOT PHOBOSINTEGRATION_LIBRARY)
    message(WARNING "library directory not found")
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LOGGING_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(PHOBOSINTEGRATION DEFAULT_MSG PHOBOSINTEGRATION_INCLUDE_DIR PHOBOSINTEGRATION_LIBRARY)

if (PHOBOSINTEGRATION_FOUND)
    set(PhobosIntegration_LIBRARIES ${PHOBOSINTEGRATION_LIBRARY} )
    set(PhobosIntegration_INCLUDE_DIRS ${PHOBOSINTEGRATION_INCLUDE_DIR} )
    set(PhobosIntegration_DEFINITIONS )
else(PHOBOSINTEGRATION_FOUND)
    message(WARNING "PhobosIntegration not found")
endif()

# Tell cmake GUIs to ignore the "local" variables.
mark_as_advanced(PhobosIntegration_DIR PHOBOSINTEGRATION_LIBRARY PHOBOSINTEGRATION_LIBRARY) 