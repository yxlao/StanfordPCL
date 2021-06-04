# This sets the following variables:
# OPENNI_FOUND - True if OPENNI was found.
# OPENNI_INCLUDE_DIRS - Directories containing the OPENNI include files.
# OPENNI_LIBRARIES - Libraries needed to use OPENNI.
# OPENNI_DEFINITIONS - Compiler flags for OPENNI.
#
# -- OPENNI_FOUND: TRUE
# -- OPENNI_INCLUDE_DIRS: /home/yixing/openni_install/usr/include/ni
# -- OPENNI_LIBRARIES: /home/yixing/openni_install/usr/lib/libOpenNI.so


# include(ExternalProject)

# if(MSVC)
#     set(lib_name libpng16_static)
# else()
#     set(lib_name png16)
# endif()

# ExternalProject_Add(
#     ext_libpng
#     PREFIX libpng
#     GIT_REPOSITORY https://github.com/glennrp/libpng.git
#     GIT_TAG v1.6.37
#     UPDATE_COMMAND ""
#     CMAKE_ARGS
#         -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#         -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
#         -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#         -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
#         -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
#         -DCMAKE_C_COMPILER_LAUNCHER=${CMAKE_C_COMPILER_LAUNCHER}
#         -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
#         -DCMAKE_POSITION_INDEPENDENT_CODE=ON
#         -DPNG_SHARED=OFF
#         -DPNG_EXECUTABLES=OFF
#         -DPNG_TESTS=OFF
#         -DPNG_BUILD_ZLIB=ON # Prevent libpng from calling find_pacakge(zlib).
#         -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIRS}
#     BUILD_BYPRODUCTS
#         <INSTALL_DIR>/${Open3D_INSTALL_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}${CMAKE_STATIC_LIBRARY_SUFFIX}
#         <INSTALL_DIR>/${Open3D_INSTALL_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib_name}d${CMAKE_STATIC_LIBRARY_SUFFIX}
# )

# ExternalProject_Get_Property(ext_libpng INSTALL_DIR)
# set(LIBPNG_INCLUDE_DIRS ${INSTALL_DIR}/include/) # "/" is critical.
# set(LIBPNG_LIB_DIR ${INSTALL_DIR}/${Open3D_INSTALL_LIB_DIR})
# set(LIBPNG_LIBRARIES ${lib_name}$<$<CONFIG:Debug>:d>)
