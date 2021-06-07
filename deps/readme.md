# StanfordPCL dependencies

## System info

- gcc 7.5, 4.8
- CUDA 11
- CMake 3.20

## Dependencies

### Boost

- `1.50.0`: download the zip file from official website, the tar file does not
  contain build scripts
- requires `toolset=gcc-4.8`

### Eigen

- `3.0.0`: https://gitlab.com/libeigen/eigen/-/archive/3.0.4/eigen-3.0.4.zip

### Flann

- `1.8.0`: https://github.com/flann-lib/flann/releases/tag/1.8.0-src
- modified
  - fix cmake target with no source file to support newer cmake

### OpenCV

- `2.3.1`: https://github.com/opencv/opencv/archive/refs/tags/2.3.1.tar.gz

### OpenNI

- `1.5.7.10`: https://github.com/OpenNI/OpenNI/releases/tag/Stable-1.5.7.10
- modified
  - fix `equivalent -> is_equivalent` to support newer compiler

### SuiteSparse

- `5.10.1`: https://github.com/DrTimothyAldenDavis/SuiteSparse/archive/refs/tags/v5.10.1.tar.gz

### VTK

- `v5.6.1`: https://github.com/Kitware/VTK/archive/refs/tags/v5.6.1.zip
- modified
  - added `#include <cstddef>` patch
  - requires gcc 4.8
