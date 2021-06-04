include(ExternalProject)

ExternalProject_Add(
    ext_flann
    PREFIX flann
    URL ${PROJECT_SOURCE_DIR}/flann/flann
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DBUILD_MATLAB_BINDINGS=OFF
        -DBUILD_PYTHON_BINDINGS=OFF
)
