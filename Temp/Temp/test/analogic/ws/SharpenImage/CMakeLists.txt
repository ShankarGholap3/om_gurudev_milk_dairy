#-------------------------------------------------------------------------------
# CMake build file for VTK Qt GUI application
# Qritten By A. Oughton
# (c) Analogic Corp  - 2018
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0 )

project(QtVtkViewer)


#-------------------------------------------------------------------------------
# VTK DIR
set(VTK_DIR ../../../../../3PBLD/VTK-8.1.0-inst/lib/cmake/vtk-8.1
             CACHE PATH "VTK directory override" FORCE)
#-------------------------------------------------------------------------------



set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# List source files & resources
#file (GLOB_RECURSE Sources *.cpp)
#file (GLOB_RECURSE Headers *.h)


set(Sources 
   main.cpp
   MainWindow.cpp
   SceneWidget.cpp
   ../common/SelectFromListDialog.cpp 
   ../common/TextInfoDialog.cpp
   ../common/InputDialog3D.cpp
   ../common/ScopedTimer.cpp
)

set(Headers 
  MainWindow.h
  SceneWidget.h
  ../common/SelectFromListDialog.h 
  ../common/TextInfoDialog.h
  ../common/InputDialog3D.h
  ../common/ScopedTimer.h
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
include_directories(
  ${VTK_INCLUDE_DIRS}
  ../common/
  )

# Build executable
add_executable(QtVtkViewer  ${Sources} ${Headers} ${Resources} ${UIs})

# Link libraries
target_link_libraries(QtVtkViewer Qt5::Widgets Qt5::OpenGL ${VTK_LIBRARIES})
