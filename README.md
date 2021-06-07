# StanfordPCL

Fork of Qianyi's StanfordPCL to work on Ubuntu 18.04 + CUDA 11.

## Formatting

```bash
# CRLF to LF
find . -not \( -name .svn -prune -o -name .git -prune \) -type f -exec dos2unix {} \;

# Remove trailing space
find . -not \( -name .svn -prune -o -name .git -prune \) -type f -print0 | xargs -0 perl -pi -e 's/ +$//'

# clang-format
echo "BasedOnStyle: LLVM\nIndentWidth: 4\nSortIncludes: false" > .clang-format
find 2d 3rdparty apps cmake common cuda doc examples features filters geometry \
    gpu io kdtree keypoints ml mobile_apps octree outofcore recognition \
    registration sample_consensus search segmentation simulation stereo \
    surface test tools tracking visualization \
    -iname *.h -o -iname *.cpp -o -iname *.cc -o -iname *.cu -o -iname *.hpp -o -iname *.cuh | xargs clang-format-10 -i
```

```txt
===============================================================================
=                          Qianyi Zhou's PCL Fork                             =
===============================================================================

I have been receiving requests for the source code of
pcl_kinfu_largeScale_release.exe, which is a critical module in the robust
scene reconstruction system we have developed.

Project: http://qianyi.info/scene.html
Code: https://github.com/qianyizh/ElasticReconstruction
Executable system: http://redwood-data.org/indoor/tutorial.html

Thus I publish my fork of PCL here as a reference.

===============================================================================

Disclaimer

I forked PCL from an early development version three years ago, and have made
numerous changes. This repository is an image of my personal development chunk.
It contains many experimental functions and redundant code.

THERE IS NO GUARANTEE THIS CODE CAN BE COMPILED OR USED. WE DO NOT INTEND TO
PROVIDE ANY SUPPORT FOR THIS CODE. IT SHOULD BE USED ONLY AS A REFERENCE.

If you are looking for the official PCL, go to this repository:
https://github.com/PointCloudLibrary/pcl

===============================================================================

License

As part of the scene reconstruction system, the code of this repository is
released under the MIT license.

Some parts of this repository are from different libraries:

Original PCL - BSD license
SuiteSparse - LGPL3+ license (we have a static library linked by Visual Studio)
Eigen - MPL2 license (we have a copy of a certain version of Eigen)
```
