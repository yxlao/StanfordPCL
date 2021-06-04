include(ExternalProject)

ExternalProject_Add(
    ext_eigen
    PREFIX eigen
    URL ${PROJECT_SOURCE_DIR}/eigen/eigen
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
