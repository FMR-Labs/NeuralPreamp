cmake_minimum_required(VERSION 3.14)
project(NeuralPreamp VERSION 1.0.0)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Discover IPLUG2_DIR if not already set (for independent builds)
if(NOT DEFINED IPLUG2_DIR)
  set(IPLUG2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." CACHE PATH "iPlug2 root directory")
endif()

# Include iPlug2 CMake configuration
include(${IPLUG2_DIR}/iPlug2.cmake)

# IGraphics backend/renderer can be set via command line:
#   cmake -DIGRAPHICS_BACKEND=SKIA -DIGRAPHICS_RENDERER=GL3 ..
# Defaults: NANOVG/METAL on macOS, NANOVG/GL2 on Windows
# See iPlug2/Scripts/cmake/IGraphics.cmake for all options

find_package(iPlug2 REQUIRED)

# Build VST3 format
# To include AUv3: FORMATS ALL_AUV3
# To select specific formats: FORMATS APP VST3 AU
iplug_add_plugin(${PROJECT_NAME}
  FORMATS VST3
  SOURCES
    NeuralPreamp.cpp
    NeuralPreamp.h
    resources/resource.h
	../FMR_Library/FMR_Lib/FMR_Neural.cpp
	../FMR_Library/FMR_NeuralAmp/FMR_NeuralAmp.cpp
	../FMR_Library/FMR_Lib/FMR_DSP_float.cpp
    ${IPLUG2_DIR}/WDL/resample.cpp
)

# Force C++20 for the VST3 target
set_property(TARGET NeuralPreamp-vst3 PROPERTY CXX_STANDARD 20)
set_property(TARGET NeuralPreamp-vst3 PROPERTY CXX_STANDARD_REQUIRED ON)

target_sources(NeuralPreamp-vst3 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/main.rc"
)
