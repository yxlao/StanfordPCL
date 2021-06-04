# This sets the following variables:
# OPENNI_FOUND - True if OPENNI was found.
# OPENNI_INCLUDE_DIRS - Directories containing the OPENNI include files.
# OPENNI_LIBRARIES - Libraries needed to use OPENNI.
# OPENNI_DEFINITIONS - Compiler flags for OPENNI.
#
# -- OPENNI_FOUND: TRUE
# -- OPENNI_INCLUDE_DIRS: /home/yixing/openni_install/usr/include/ni
# -- OPENNI_LIBRARIES: /home/yixing/openni_install/usr/lib/libOpenNI.so

include(ExternalProject)

ExternalProject_Add(
    ext_openni
    PREFIX openni
    URL ${PROJECT_SOURCE_DIR}/openni/openni
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_DIR ${DEPS_INSTALL_PREFIX}
    BUILD_COMMAND   cd Platform/Linux/Build && make -j6
    COMMAND         cd Platform/Linux/CreateRedist && ./RedistMaker
    INSTALL_COMMAND echo "OpenNI will be installed to" <INSTALL_DIR>
    COMMAND         rm -rf <INSTALL_DIR>/usr <INSTALL_DIR>/var
    COMMAND         mkdir -p <INSTALL_DIR>/usr/lib <INSTALL_DIR>/usr/bin <INSTALL_DIR>/var
    COMMAND         cd Platform/Linux/Redist/OpenNI-Bin-Dev-Linux-x64-v1.5.7.10 && ./install.sh -c <INSTALL_DIR>
    COMMAND         eval mv <INSTALL_DIR>/usr/* <INSTALL_DIR>
    COMMAND         rm -rf <INSTALL_DIR>/var <INSTALL_DIR>/usr
)
