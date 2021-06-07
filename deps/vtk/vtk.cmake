# This sets the following variables:
# VTK_FOUND
# VTK_INCLUDE_DIRS
# VTK_LIBRARY_DIRS

include(ExternalProject)

ExternalProject_Add(
    ext_vtk
    PREFIX vtk
    URL https://github.com/Kitware/VTK/archive/refs/tags/v5.6.1.zip
    PATCH_COMMAND patch Utilities/vtkmetaio/metaUtils.cxx < ${PROJECT_SOURCE_DIR}/vtk/patch_file.diff
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_C_COMPILER=gcc-4.8
        -DCMAKE_CXX_COMPILER=g++-4.8
        -DCMAKE_BUILD_TYPE="Release"
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY
        -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY
        -Wno-dev
)
