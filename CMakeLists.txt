cmake_minimum_required(VERSION 3.9)

# Define project
project(I3DRSGM VERSION 1.0.0 DESCRIPTION "I3DR's Semi-Global Matching library" LANGUAGES CXX)

# Use 'Release' by default for build
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Define GNU standard installation directories
include(GNUInstallDirs)

# Add export definitions
add_definitions(-DI3DRSGM_EXPORT)

# Define library source files
set(SOURCE_FILES src/i3drsgm.cpp)

# Use FindXXX.cmake files for finding libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Define PhobosIntegration library
find_package(PhobosIntegration REQUIRED)
include_directories(${PhobosIntegration_INCLUDE_DIRS})

# Build library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${PhobosIntegration_LIBRARIES})

# Include headers in library
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

# Include version in library
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

# Export cmake config
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install headers in install folder
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Install cmake config in install folder
install(EXPORT ${PROJECT_NAME}Config DESTINATION lib/cmake/${PROJECT_NAME})

# Install cmake targets in install folder
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

# Install param files to bin folder
install(DIRECTORY params/ DESTINATION bin)