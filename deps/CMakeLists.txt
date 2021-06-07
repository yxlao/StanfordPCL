cmake_minimum_required(VERSION 3.10)

project(PCL_DEPS)

# All external projects shall be installed to here
set(PCL_DEPS_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# OpenNI
# Force OpenNI to build first, as it requires special treatment for install dir
include(${PROJECT_SOURCE_DIR}/openni/openni.cmake)

# Boost
include(${PROJECT_SOURCE_DIR}/boost/boost.cmake)
add_dependencies(ext_boost ext_openni)

# FLANN
include(${PROJECT_SOURCE_DIR}/flann/flann.cmake)
add_dependencies(ext_flann ext_openni)

# Eigen
include(${PROJECT_SOURCE_DIR}/eigen/eigen.cmake)
add_dependencies(ext_eigen ext_openni)

# OpenCV
include(${PROJECT_SOURCE_DIR}/opencv/opencv.cmake)
add_dependencies(ext_opencv ext_openni)

# Qhull
include(${PROJECT_SOURCE_DIR}/qhull/qhull.cmake)
add_dependencies(ext_qhull ext_openni)

# Suitesparse
include(${PROJECT_SOURCE_DIR}/suitesparse/suitesparse.cmake)
add_dependencies(ext_suitesparse ext_openni)

# VTK
include(${PROJECT_SOURCE_DIR}/vtk/vtk.cmake)
add_dependencies(ext_vtk ext_openni)
