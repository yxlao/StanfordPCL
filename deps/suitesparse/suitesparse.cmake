include(ExternalProject)

ExternalProject_Add(
    ext_suitesparse
    PREFIX suitesparse
    URL https://github.com/DrTimothyAldenDavis/SuiteSparse/archive/refs/tags/v5.10.1.tar.gz
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CONFIGURE_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_COMMAND make  # make -j is not helpful here
    BUILD_IN_SOURCE ON
    make install INSTALL=<INSTALL_DIR>
)
