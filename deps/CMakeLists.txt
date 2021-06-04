cmake_minimum_required(VERSION 3.10)

project(PCL_DEPS)

# All external projects shall be installed to here
set(DEPS_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# OpenNI
# Force OpenNI to build first, as it requires special treatment for install dir
include(${PROJECT_SOURCE_DIR}/openni/openni.cmake)

# FLANN
include(${PROJECT_SOURCE_DIR}/flann/flann.cmake)
add_dependencies(ext_flann ext_openni)
