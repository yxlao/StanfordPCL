cmake_minimum_required(VERSION 3.10)

project(PCL_DEPS)

# All external projects shall be installed to here
set(DEPS_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# OpenNI
# Build this first, as this requires special treatment of install dir.
include(${PROJECT_SOURCE_DIR}/openni/openni.cmake)

# FLANN

