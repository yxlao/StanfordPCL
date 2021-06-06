# include(ExternalProject)
# include(ProcessorCount)
# ProcessorCount(NPROC)

# ExternalProject_Add(
#     ext_boost
#     PREFIX boost
#     URL ${PROJECT_SOURCE_DIR}/boost/boost_1_50_0.tar.bz2
#     INSTALL_DIR ${DEPS_INSTALL_PREFIX}
#     BUILD_IN_SOURCE ON
#     UPDATE_COMMAND ""
#     CONFIGURE_COMMAND ./bootstrap.sh -prefix=<INSTALL_DIR>
#     BUILD_COMMAND     ./b2 toolset=gcc-4.8 -j ${NPROC}
#     INSTALL_COMMAND   ./b2 toolset=gcc-4.8 install -j ${NPROC}
# )

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(NPROC)

ExternalProject_Add(
    ext_boost
    PREFIX boost
    DOWNLOAD_COMMAND wget https://github.com/boostorg/boost/archive/refs/tags/boost-1.50.0.zip # boost-boost
    COMMAND          unzip boost-1.50.0.zip && rm -rf boost-1.50.0.zip
    COMMAND          wget https://github.com/boostorg/build/archive/refs/tags/boost-1.50.0.zip # boost-build
    COMMAND          unzip boost-1.50.0.zip && rm -rf boost-1.50.0.zip
    COMMAND          rm -rf ext_boost && mkdir ext_boost
    COMMAND          eval mv boost-boost-1.50.0/* ext_boost
    COMMAND          eval mv build-boost-1.50.0/v2 ext_boost/tools/build
    COMMAND          rm -rf boost-boost-1.50.0/ build-boost-1.50.0/
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    BUILD_IN_SOURCE ON
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ./bootstrap.sh -prefix=<INSTALL_DIR>
    BUILD_COMMAND ./b2 toolset=gcc-4.8 -j ${NPROC}
    INSTALL_COMMAND ./b2 toolset=gcc-4.8 install -j ${NPROC}
)
