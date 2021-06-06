# StanfordPCL dependencies

## System info

- gcc 7.5, 4.8
- CUDA 11
- CMake 3.20

## Dependencies

### Eigen

- `3.0.0`: https://gitlab.com/libeigen/eigen/-/archive/3.0.4/eigen-3.0.4.zip

### Flann

- `1.8.0`: https://github.com/flann-lib/flann/releases/tag/1.8.0-src
- modified
  - fix cmake target with no source file to support newer cmake

### OpenNI

- `1.5.7.10`: https://github.com/OpenNI/OpenNI/releases/tag/Stable-1.5.7.10
- modified
  - fix `equivalent -> is_equivalent` to support newer compiler

### VTK

- `v5.6.1`: https://github.com/Kitware/VTK/archive/refs/tags/v5.6.1.zip
- modified
  - added `#include <cstddef>` patch

### OpenCV

- `2.3.1`: https://github.com/opencv/opencv/archive/refs/tags/2.3.1.tar.gz
- requires `toolset=gcc-4.8`

### SuiteSparse

- `5.10.1`: https://github.com/DrTimothyAldenDavis/SuiteSparse/archive/refs/tags/v5.10.1.tar.gz
