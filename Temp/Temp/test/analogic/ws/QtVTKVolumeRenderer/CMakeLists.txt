#-------------------------------------------------------------------------------
# CMake build file for Simple Volume Renderring  VTK Qt GUI application
# Written By A. Oughton
# (c) Analogic Corp  - 2018
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0 )

project(QVTKOpenGLWidgetRenderer C CXX)



#-------------------------------------------------------------------------------
# VTK DIR
set(VTK_DIR  /home/analogic/AT3D_ravesilver4/workstation/3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-8.1
             CACHE PATH "VTK directory override" FORCE)
#-------------------------------------------------------------------------------



set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# With this definition I can select the class I build width
add_definitions(-DUSE_OPENGL_VTK_WIDGET)


# List source files & resources
#file (GLOB_RECURSE Sources *.cpp)
#file (GLOB_RECURSE Headers *.h)

set(Sources
 main.cpp
 MainWindow.cpp
 OpenGLVolumeWidget.cpp
 ../common/LocalVRUtils.cpp
 ../common/LocalTouchHandler.cpp
 )

set (Headers
 MainWindow.h
 OpenGLVolumeWidget.h
 ../common/LocalVRUtils.h
 ../common/LocalScannerDefs.h
 ../common/LocalTouchHandler.h
)


file (GLOB_RECURSE Resources *.qrc)
file (GLOB_RECURSE UIs *.ui)

# Find package Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

# Use Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
qt5_add_resources(Resources ${Resources})

# Find package VTK
find_package(VTK REQUIRED)

# Use VTK
include(${VTK_USE_FILE})
include_directories(${VTK_INCLUDE_DIRS}
 ../common
)

# Build executable
add_executable(QVTKOpenGLWidgetRenderer
   ${Sources}
   ${Headers}
   ${Resources}
   ${UIs}
)

# Link libraries
target_link_libraries(QVTKOpenGLWidgetRenderer
          Qt5::Widgets
          Qt5::OpenGL
          ${VTK_LIBRARIES}
)

