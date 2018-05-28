# ----------------------------------------------------------------------------
#   Basic Configuration
# ----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)


project(ArUcoGUI VERSION "0.0.1" LANGUAGES CXX)
set(PROJECT_SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON) #...with compiler extensions like gnu++11
set(CMAKE_CXX_STANDARD 11) # C++11...

#------------------------------------------------------
# Lib Names and Dirs
#------------------------------------------------------
if(WIN32)
    # Postfix of DLLs:
    SET(PROJECT_DLLVERSION "${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_PATCH}")
    SET(RUNTIME_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for dlls and binaries")
    SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for binaries")
    SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for dlls")
else()
    # Postfix of so's:
    set(PROJECT_DLLVERSION)
endif()

include(cmake/options.cmake)
include(cmake/compiler.cmake)
include(cmake/dependencies.cmake)


#################################
#  Find and Configure Qt5

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
#################################
# Add modules
#################################
ADD_SUBDIRECTORY(3rdparty/aruco/aruco)
include_directories(3rdparty/aruco)

include_directories(. ./tools)


SET(BUILD_QT ON)
add_subdirectory(tools/gparam)
add_subdirectory(tools/moduletools)
add_subdirectory(modulecalibration)
add_subdirectory(modulearucotest)
SET(USED_MODULES  modulecalibration modulearucotest  )


#   add_subdirectory(qtgl)
#    add_subdirectory(appmodules/scannercontrol)


#################################
# Add main files
#################################1

SET(qtproject_SRCS  main.cpp mainwindow.cpp     )
SET(qtproject_HDRS  mainwindow.h  arucogparam.h  )


#################################
# Creation rules

QT5_WRAP_UI(qtproject_UIS_HDRS ${qtproject_UIS} )
qt5_add_resources(qtproject_RCC appresources.qrc)

add_executable(${PROJECT_NAME}  ${qtproject_UIS_HDRS} ${qtproject_SRCS}  ${qtproject_RCC} ${qtproject_HDRS} ${qtproject_UIS_HDRS})
qt5_use_modules(${PROJECT_NAME}  Widgets  )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}   ${QT_LIBRARIES}  ${REQUIRED_LIBRARIES} ${USED_MODULES}  opencv_core opencv_imgproc aruco moduletools gparam gparam_qt  )
INSTALL(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)


# ----------------------------------------------------------------------------
#   Uninstall target, for "make uninstall"
# ----------------------------------------------------------------------------
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
# ----------------------------------------------------------------------------
# create configuration file from .in file (If you use windows take care with paths)
# ----------------------------------------------------------------------------
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/cmake/config.cmake.in" "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
INSTALL(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" DESTINATION share/${PROJECT_NAME} )


include(cmake/display.cmake)
