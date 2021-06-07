include(ExternalProject)

ExternalProject_Add(
    ext_qhull
    PREFIX qhull
    URL https://github.com/qhull/qhull/archive/refs/tags/2011.2.zip
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
)
