cmake_minimum_required(VERSION 3.10)

project(PCL_DEPS)

# All external projects shall be installed to here
# Equivalent directories as the one specified in the top-level CMakeLists.txt
set(PCL_DEPS_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# Boost
include(${PROJECT_SOURCE_DIR}/boost/boost.cmake)

# FLANN
include(${PROJECT_SOURCE_DIR}/flann/flann.cmake)

# Eigen
include(${PROJECT_SOURCE_DIR}/eigen/eigen.cmake)

# OpenCV
include(${PROJECT_SOURCE_DIR}/opencv/opencv.cmake)

# Qhull
include(${PROJECT_SOURCE_DIR}/qhull/qhull.cmake)

# Suitesparse
include(${PROJECT_SOURCE_DIR}/suitesparse/suitesparse.cmake)

# VTK
include(${PROJECT_SOURCE_DIR}/vtk/vtk.cmake)
