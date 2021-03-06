
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(gem-xray)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
find_package(ROOT REQUIRED)

set(Garfield_DIR $ENV{GARFIELD_HOME})
set(Garfield_INC_DIR $ENV{GARFIELD_HOME}/include)
set(Garfield_LIB_DIR $ENV{GARFIELD_HOME}/lib)
set(Heed_DIR $ENV{GARFIELD_HOME}/Heed)
set(Garfield_LIBRARIES Garfield)

message(STATUS "Found Garfield in ${Garfield_DIR}")
message(STATUS "Found Garfield libs in ${Garfield_LIB_DIR}")
message(STATUS "Found HEED in ${Heed_DIR}")


#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/root-style)
include_directories(${Garfield_INC_DIR})
include_directories(${Heed_INC_DIR})

link_directories(${Garfield_LIB_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(gem-xray gem-xray.cc ${sources} ${headers})
target_link_libraries(gem-xray ${Geant4_LIBRARIES})
target_link_libraries(gem-xray ${ROOT_LIBRARIES})
target_link_libraries(gem-xray ${Garfield_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEB1_SCRIPTS
  init_vis.mac
  run1.mac
  run2.mac
  run.mac
  vis.mac
  xray-spectrum.csv
  xray-spectrum-40kV.csv
  analysis.py
  )

foreach(_script ${EXAMPLEB1_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(B1 DEPENDS exampleB1)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS gem-xray DESTINATION bin)


