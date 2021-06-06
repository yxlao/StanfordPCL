include(ExternalProject)

ExternalProject_Add(
    ext_opencv
    PREFIX opencv
    URL https://github.com/opencv/opencv/archive/refs/tags/2.3.1.tar.gz
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_CXX_FLAGS=-Wno-narrowing
        -DWITH_FFMPEG=OFF
        -DWITH_CUDA=OFF
)
