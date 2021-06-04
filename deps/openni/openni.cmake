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
    URL ${PROJECT_SOURCE_DIR}/deps/openni/openni
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND ""
    UPDATE_COMMAND ""
    BUILD_COMMAND cd Platform/Linux/Build && make
    INSTALL_COMMAND ""
)
add_dependencies(ext_openni always_rebuild)

ExternalProject_Get_Property(ext_openni INSTALL_DIR)
set(OPENNI_FOUND TRUE)
set(OPENNI_INCLUDE_DIRS ${INSTALL_DIR}) # "/" is critical.
set(OPENNI_LIBRARIES ${INSTALL_DIR}})
