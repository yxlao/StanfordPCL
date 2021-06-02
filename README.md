# Qianyi Zhou's PCL Fork

## Installation

### Boost

1. Download Boost 1.70 from https://www.boost.org/users/history/version_1_70_0.html
2. Extract to C:\Program Files\boost
   - That is, you should have C:\Program Files\boost\boost_1_70_0\bootstrap.bat
3. Now switch to a "Visual Studio Developer Command Prompt". Search
   "Developer Command Prompt" from the start menu. Make sure `cl` is available
   from your terminal. Newer versions of Boost can detect `cl` automatically.
   Typiclaly, `cl` comes with Visual Studio installation, e.g. in
   `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\bin\Hostx86\x86`
4. Build boost.
   ```ps1
   $env:BOOST_INCLUDEDIR = "C:\Program Files\boost\boost_1_70_0\";
   $env:BOOST_LIBRARYDIR = "C:\Program Files\boost\boost_1_70_0\stage\lib";
   $env:BOOST_ROOT       = "C:\Program Files\boost\boost_1_70_0";
   .\bootstrap.bat
   .\b2.exe -j 10
   ```
   To clean build
   ```ps1
   .\b2.exe --clean-all -n
   ```

## Introduction

I have been receiving requests for the source code of
pcl_kinfu_largeScale_release.exe, which is a critical module in the robust
scene reconstruction system we have developed.

Project: http://qianyi.info/scene.html
Code: https://github.com/qianyizh/ElasticReconstruction
Executable system: http://redwood-data.org/indoor/tutorial.html

Thus I publish my fork of PCL here as a reference.

## Disclaimer

I forked PCL from an early development version three years ago, and have made
numerous changes. This repository is an image of my personal development chunk.
It contains many experimental functions and redundant code.

THERE IS NO GUARANTEE THIS CODE CAN BE COMPILED OR USED. WE DO NOT INTEND TO
PROVIDE ANY SUPPORT FOR THIS CODE. IT SHOULD BE USED ONLY AS A REFERENCE.

If you are looking for the official PCL, go to this repository:
https://github.com/PointCloudLibrary/pcl

## License

As part of the scene reconstruction system, the code of this repository is
released under the MIT license.

Some parts of this repository are from different libraries:

Original PCL - BSD license
SuiteSparse - LGPL3+ license (we have a static library linked by Visual Studio)
Eigen - MPL2 license (we have a copy of a certain version of Eigen)
