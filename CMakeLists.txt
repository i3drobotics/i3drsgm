cmake_minimum_required(VERSION 3.9)

option(BUILD_CSHARP "Build C#" ON)

# Define project
if(BUILD_CSHARP)
    project(I3DRSGM VERSION 1.0.6 DESCRIPTION "I3DR's Semi-Global Matching library" LANGUAGES CXX CSharp)
    if(NOT MSVC)
        message(FATAL_ERROR "This CMake files only works with MSVC due to building C#.")
    endif(NOT MSVC)
else(BUILD_CSHARP)
    project(I3DRSGM VERSION 1.0.6 DESCRIPTION "I3DR's Semi-Global Matching library" LANGUAGES CXX)
endif(BUILD_CSHARP)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

option(BUILD_APP "Build app" ON)
if(BUILD_APP)
    message("BUILD APP: ON")
else()
    message("BUILD APP: OFF")
endif()

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
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Define library source files
set(LIB_SRC_FILES src/i3drsgm.cpp)
if(BUILD_APP)
    set(APP_SRC_FILES src/i3drsgmApp.cpp)
endif(BUILD_APP)

if(BUILD_CSHARP)
    set(CSHARP_SRC_FILES src/I3DRSGMSharp.cs)
    if(BUILD_APP)
        set(CSHARP_APP_SRC_FILES src/I3DRSGMSharpApp.cs)
    endif(BUILD_APP)
endif(BUILD_CSHARP)
#set(SRC_FILES src/mog2.cpp)

# Use FindXXX.cmake files for finding libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Define PhobosIntegration library
find_package(PhobosIntegration REQUIRED)
include_directories(${PhobosIntegration_INCLUDE_DIRS})

# Build library
add_library(lib${PROJECT_NAME} SHARED ${LIB_SRC_FILES})
target_link_libraries(lib${PROJECT_NAME} ${PhobosIntegration_LIBRARIES})
target_compile_definitions(lib${PROJECT_NAME} PRIVATE I3DRSGM_EXPORT)

if(BUILD_APP)
    # Build simple app
    add_executable(${PROJECT_NAME}App ${APP_SRC_FILES})
    target_link_libraries(${PROJECT_NAME}App PRIVATE lib${PROJECT_NAME})
endif(BUILD_APP)

if(BUILD_CSHARP)
    # Build simple app
    add_library(${PROJECT_NAME_UPPER}Sharp SHARED ${CSHARP_SRC_FILES})
    set_property(TARGET ${PROJECT_NAME_UPPER}Sharp PROPERTY VS_DOTNET_REFERENCES
        "Microsoft.CSharp"
        "System"
        "System.Diagnostics"
    )
    if(BUILD_APP)
        add_executable(${PROJECT_NAME_UPPER}SharpApp ${CSHARP_APP_SRC_FILES})
        target_link_libraries(${PROJECT_NAME_UPPER}SharpApp PRIVATE ${PROJECT_NAME_UPPER}Sharp)
    endif(BUILD_APP)
endif(BUILD_CSHARP)

# Include headers in library
target_include_directories(lib${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

# Include version in library
set_target_properties(lib${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION})

# Export cmake config
install(TARGETS lib${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

# Export cmake config
install(TARGETS ${PROJECT_NAME_UPPER}Sharp EXPORT ${PROJECT_NAME_UPPER}SharpConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install headers in install folder
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Install cmake config in install folder
install(EXPORT ${PROJECT_NAME}Config DESTINATION lib/cmake/${PROJECT_NAME})

# Install cmake targets in install folder
export(TARGETS lib${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

if(BUILD_APP)
    # Install app in bin folder
    install(TARGETS ${PROJECT_NAME}App DESTINATION bin)
    if(BUILD_CSHARP)
        install(TARGETS ${PROJECT_NAME_UPPER}SharpApp DESTINATION bin)
    endif(BUILD_CSHARP)
endif(BUILD_APP)

# Define param files
file(GLOB PARAM_FILES
    "${PROJECT_SOURCE_DIR}/params/*.param"
)

add_custom_command(TARGET lib${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PARAM_FILES} $<TARGET_FILE_DIR:lib${PROJECT_NAME}>
    COMMENT "Copying files to build..."
)

install(FILES ${PARAM_FILES} DESTINATION bin)

if(BUILD_APP)
    # Define OpenCV dlls (will be copied to install folder)
    set(OpenCV_DLLS
        ${OpenCV_DIR}/x64/vc15/bin/opencv_world450.dll
        ${OpenCV_DIR}/x64/vc15/bin/opencv_videoio_ffmpeg450_64.dll
        ${OpenCV_DIR}/x64/vc15/bin/opencv_videoio_msmf450_64.dll
    )
    # Define resource files (will be copied to install folder)
    file(GLOB RESOURCE_FILES
        "${PROJECT_SOURCE_DIR}/resources/*.png"
        "${PROJECT_SOURCE_DIR}/resources/*.yaml"
    )

    # QT dlls required if not using your own QT dlls
    # seperated into a different folder to avoid conficts
    file(GLOB PhobosIntegration_DLLS
        "${PhobosIntegration_DIR}/bin/*.dll"
        "${PhobosIntegration_DIR}/qt/*.dll"
    )

    # Define python files
    file(GLOB PYTHON_FILES
        "${PROJECT_SOURCE_DIR}/py/*.py"
    )

    # Copy files to build directory
    # add_custom_command(TARGET ${PROJECT_NAME}App POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OpenCV_DLLS} $<TARGET_FILE_DIR:${PROJECT_NAME}App>
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PhobosIntegration_DLLS} $<TARGET_FILE_DIR:${PROJECT_NAME}App>
    #     COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}App>/resources
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}App>/resources
    #     COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}App>/py
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PYTHON_FILES} $<TARGET_FILE_DIR:${PROJECT_NAME}App>/py
    #     COMMENT "Copying extra files to build..."
    # )

    # Install OpenCV dlls in install directory
    install(FILES ${OpenCV_DLLS} DESTINATION bin)
    # Install PhobosIntegration dlls in install directory
    install(FILES ${PhobosIntegration_DLLS} DESTINATION bin)
    # Install resource files in install directory
    install(DIRECTORY DESTINATION bin/resources)
    install(FILES ${RESOURCE_FILES} DESTINATION bin/resources)
    # Install resource files in py directory
    install(FILES ${PYTHON_FILES} DESTINATION bin/py)

endif()