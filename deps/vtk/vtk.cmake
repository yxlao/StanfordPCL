# This sets the following variables:
# VTK_FOUND
# VTK_INCLUDE_DIRS
# VTK_LIBRARY_DIRS

include(ExternalProject)

ExternalProject_Add(
    ext_vtk
    PREFIX vtk
    URL ${PROJECT_SOURCE_DIR}/deps/vtk/VTK
    PATCH_COMMAND patch Utilities/vtkmetaio/metaUtils.cxx < ${PROJECT_SOURCE_DIR}/deps/vtk/patch_file.diff
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_C_COMPILER=gcc-4.8
        -DCMAKE_CXX_COMPILER=g++-4.8
        -DCMAKE_BUILD_TYPE="Release"
        -DBUILD_SHARED_LIBS=ON
        -DBUILD_TESTING=OFF
        -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY
        -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY
        -Wno-dev
)
add_dependencies(ext_vtk always_rebuild)

ExternalProject_Get_Property(ext_vtk INSTALL_DIR)
set(VTK_FOUND TRUE)
set(VTK_INCLUDE_DIRS ${INSTALL_DIR}/include)
set(VTK_LIBRARY_DIRS ${INSTALL_DIR}/lib)
