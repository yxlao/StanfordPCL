include(ExternalProject)

ExternalProject_Add(
    ext_flann
    PREFIX flann
    GIT_REPOSITORY https://github.com/flann-lib/flann.git
    GIT_TAG 1.8.0-src
    INSTALL_DIR ${PCL_DEPS_INSTALL_PREFIX}
    UPDATE_COMMAND ""
    PATCH_COMMAND git apply ${PROJECT_SOURCE_DIR}/flann/0001-add-empty.cpp-and-upate-cmake.patch
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DBUILD_MATLAB_BINDINGS=OFF
        -DBUILD_PYTHON_BINDINGS=OFF
)
