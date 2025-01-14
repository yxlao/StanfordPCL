/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2011, Willow Garage, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef PCL_KINFU_INTERNAL_HPP_
#define PCL_KINFU_INTERNAL_HPP_

#include <pcl/gpu/containers/device_array.h>
//#include <pcl/gpu/utils/safe_call.hpp>
#include "safe_call.hpp"

#include <pcl/gpu/kinfu_large_scale/tsdf_buffer.h>

namespace pcl {
namespace device {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Types
typedef unsigned short ushort;
typedef DeviceArray2D<float> MapArr;
typedef DeviceArray2D<ushort> DepthMap;
typedef float4 PointType;

// Tsdf fixed point divisor (if old format is enabled)
// const int DIVISOR = 2047;     // SHRT_MAX;
const int DIVISOR = 32767; // SHRT_MAX;

// RGB images resolution
const float HEIGHT = 480.0f;
const float WIDTH = 640.0f;

// should be multiple of 32
enum { VOLUME_X = 512, VOLUME_Y = 512, VOLUME_Z = 512 };
// enum { VOLUME_X = 512, VOLUME_Y = 512, VOLUME_Z = 1024 };
// enum { VOLUME_X = 392, VOLUME_Y = 392, VOLUME_Z = 392 };

// temprary constant (until we make it automatic) that holds the Kinect's focal
// lenght
const float FOCAL_LENGTH = 575.816f;

const float VOLUME_SIZE =
    3.0f; // physical size represented by the TSDF volume. In meters
const float DISTANCE_THRESHOLD =
    1.5f; // when the camera target point is farther than DISTANCE_THRESHOLD
          // from the current cube's center, shifting occurs. In meters
const int SNAPSHOT_RATE =
    45; // every 45 frames an RGB snapshot will be saved. -et parameter is
        // needed when calling Kinfu Large Scale in command line.

/** \brief Camera intrinsics structure
 */
struct Intr {
    float fx, fy, cx, cy, trunc_dist;
    Intr(){};
    Intr(float fx_, float fy_, float cx_, float cy_, float trunc_dist_)
        : fx(fx_), fy(fy_), cx(cx_), cy(cy_), trunc_dist(trunc_dist_){};

    Intr operator()(int level_index) const {
        int div = 1 << level_index;
        return (Intr(fx / div, fy / div, cx / div, cy / div, trunc_dist));
    }
};

/** \brief 3x3 Matrix for device code
 */
struct Mat33 {
    float3 data[3];
};

/** \brief Light source collection
 */
struct LightSource {
    float3 pos[1];
    int number;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maps

/** \brief Perfoms bilateral filtering of disparity map
 * \param[in] src soruce map
 * \param[out] dst output map
 */
void bilateralFilter(const DepthMap &src, DepthMap &dst);

/** \brief Computes depth pyramid
 * \param[in] src source
 * \param[out] dst destination
 */
void pyrDown(const DepthMap &src, DepthMap &dst);

/** \brief Computes vertex map
 * \param[in] intr depth camera intrinsics
 * \param[in] depth depth
 * \param[out] vmap vertex map
 */
void createVMap(const Intr &intr, const DepthMap &depth, MapArr &vmap);

/** \brief Computes normal map using cross product
 * \param[in] vmap vertex map
 * \param[out] nmap normal map
 */
void createNMap(const MapArr &vmap, MapArr &nmap);

/** \brief Computes normal map using Eigen/PCA approach
 * \param[in] vmap vertex map
 * \param[out] nmap normal map
 */
void computeNormalsEigen(const MapArr &vmap, MapArr &nmap);

/** \brief Performs affine tranform of vertex and normal maps
 * \param[in] vmap_src source vertex map
 * \param[in] nmap_src source vertex map
 * \param[in] Rmat Rotation mat
 * \param[in] tvec translation
 * \param[out] vmap_dst destination vertex map
 * \param[out] nmap_dst destination vertex map
 */
void tranformMaps(const MapArr &vmap_src, const MapArr &nmap_src,
                  const Mat33 &Rmat, const float3 &tvec, MapArr &vmap_dst,
                  MapArr &nmap_dst);

/** \brief Performs depth truncation
 * \param[out] depth depth map to truncation
 * \param[in] max_distance truncation threshold, values that are higher than the
 * threshold are reset to zero (means not measurement)
 */
void truncateDepth(DepthMap &depth, float max_distance);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//   ICP

/** \brief (now it's exra code) Computes corespondances map
 * \param[in] vmap_g_curr current vertex map in global coo space
 * \param[in] nmap_g_curr current normals map in global coo space
 * \param[in] Rprev_inv inverse camera rotation at previous pose
 * \param[in] tprev camera translation at previous pose
 * \param[in] intr camera intrinsics
 * \param[in] vmap_g_prev previous vertex map in global coo space
 * \param[in] nmap_g_prev previous vertex map in global coo space
 * \param[in] distThres distance filtering threshold
 * \param[in] angleThres angle filtering threshold. Represents sine of angle
 * between normals \param[out] coresp
 */
void findCoresp(const MapArr &vmap_g_curr, const MapArr &nmap_g_curr,
                const Mat33 &Rprev_inv, const float3 &tprev, const Intr &intr,
                const MapArr &vmap_g_prev, const MapArr &nmap_g_prev,
                float distThres, float angleThres, PtrStepSz<short2> coresp);

/** \brief (now it's exra code) Computation Ax=b for ICP iteration
 * \param[in] v_dst destination vertex map (previous frame cloud)
 * \param[in] n_dst destination normal map (previous frame normals)
 * \param[in] v_src source normal map (current frame cloud)
 * \param[in] coresp Corespondances
 * \param[out] gbuf temp buffer for GPU reduction
 * \param[out] mbuf ouput GPU buffer for matrix computed
 * \param[out] matrixA_host A
 * \param[out] vectorB_host b
 */
void estimateTransform(const MapArr &v_dst, const MapArr &n_dst,
                       const MapArr &v_src, const PtrStepSz<short2> &coresp,
                       DeviceArray2D<float> &gbuf, DeviceArray<float> &mbuf,
                       float *matrixA_host, float *vectorB_host);

/** \brief Computation Ax=b for ICP iteration
 * \param[in] Rcurr Rotation of current camera pose guess
 * \param[in] tcurr translation of current camera pose guess
 * \param[in] vmap_curr current vertex map in camera coo space
 * \param[in] nmap_curr current vertex map in camera coo space
 * \param[in] Rprev_inv inverse camera rotation at previous pose
 * \param[in] tprev camera translation at previous pose
 * \param[in] intr camera intrinsics
 * \param[in] vmap_g_prev previous vertex map in global coo space
 * \param[in] nmap_g_prev previous vertex map in global coo space
 * \param[in] distThres distance filtering threshold
 * \param[in] angleThres angle filtering threshold. Represents sine of angle
 * between normals \param[out] gbuf temp buffer for GPU reduction \param[out]
 * mbuf ouput GPU buffer for matrix computed \param[out] matrixA_host A
 * \param[out] vectorB_host b
 */
void estimateCombined(const Mat33 &Rcurr, const float3 &tcurr,
                      const MapArr &vmap_curr, const MapArr &nmap_curr,
                      const Mat33 &Rprev_inv, const float3 &tprev,
                      const Intr &intr, const MapArr &vmap_g_prev,
                      const MapArr &nmap_g_prev, float distThres,
                      float angleThres, DeviceArray2D<float> &gbuf,
                      DeviceArray<float> &mbuf, float *matrixA_host,
                      float *vectorB_host);

void estimateCombinedPrevSpace(const Mat33 &Rcurr, const float3 &tcurr,
                               const MapArr &vmap_curr, const MapArr &nmap_curr,
                               const Mat33 &Rprev_inv, const float3 &tprev,
                               const Intr &intr, const MapArr &vmap_g_prev,
                               const MapArr &nmap_g_prev, float distThres,
                               float angleThres, DeviceArray2D<float> &gbuf,
                               DeviceArray<float> &mbuf, float *matrixA_host,
                               float *vectorB_host);

void estimateCombinedEx(const Mat33 &Rcurr, const Mat33 &Rcurr_t,
                        const float3 &tcurr, const MapArr &vmap_curr,
                        const MapArr &nmap_curr, const Mat33 &Rprev_inv,
                        const float3 &tprev, const Intr &intr,
                        const MapArr &vmap_g_prev, const MapArr &nmap_g_prev,
                        float distThres, float angleThres,
                        DeviceArray2D<float> &gbuf, DeviceArray<float> &mbuf,
                        float *matrixA_host, float *vectorB_host,
                        DeviceArray<float> &gbuf_slac_triangle,
                        DeviceArray<float> &gbuf_slac_block,
                        float *matrixSLAC_A_host, float *matrixSLAC_block_host);

/** \brief Computation Ax=b for ICP iteration
 * \param[in] Rcurr Rotation of current camera pose guess
 * \param[in] tcurr translation of current camera pose guess
 * \param[in] vmap_curr current vertex map in camera coo space
 * \param[in] nmap_curr current vertex map in camera coo space
 * \param[in] Rprev_inv inverse camera rotation at previous pose
 * \param[in] tprev camera translation at previous pose
 * \param[in] intr camera intrinsics
 * \param[in] vmap_g_prev previous vertex map in global coo space
 * \param[in] nmap_g_prev previous vertex map in global coo space
 * \param[in] distThres distance filtering threshold
 * \param[in] angleThres angle filtering threshold. Represents sine of angle
 * between normals \param[out] gbuf temp buffer for GPU reduction \param[out]
 * mbuf ouput GPU buffer for matrix computed \param[out] matrixA_host A
 * \param[out] vectorB_host b
 */
void estimateCombined(const Mat33 &Rcurr, const float3 &tcurr,
                      const MapArr &vmap_curr, const MapArr &nmap_curr,
                      const Mat33 &Rprev_inv, const float3 &tprev,
                      const Intr &intr, const MapArr &vmap_g_prev,
                      const MapArr &nmap_g_prev, float distThres,
                      float angleThres, DeviceArray2D<double> &gbuf,
                      DeviceArray<double> &mbuf, double *matrixA_host,
                      double *vectorB_host);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TSDF volume functions

/** \brief Perform tsdf volume initialization
 *  \param[out] array volume to be initialized
 */
PCL_EXPORTS void initVolume(PtrStep<short2> array);

// first version
/** \brief Performs Tsfg volume uptation (extra obsolete now)
 * \param[in] depth_raw Kinect depth image
 * \param[in] intr camera intrinsics
 * \param[in] volume_size size of volume in mm
 * \param[in] Rcurr_inv inverse rotation for current camera pose
 * \param[in] tcurr translation for current camera pose
 * \param[in] tranc_dist tsdf truncation distance
 * \param[in] volume tsdf volume to be updated
 */
void integrateTsdfVolume(const PtrStepSz<ushort> &depth_raw, const Intr &intr,
                         const float3 &volume_size, const Mat33 &Rcurr_inv,
                         const float3 &tcurr, float tranc_dist,
                         PtrStep<short2> volume);

// second version
/** \brief Function that integrates volume if volume element contains: 2 bytes
 * for round(tsdf*SHORT_MAX) and 2 bytes for integer weight. \param[in] depth
 * Kinect depth image \param[in] intr camera intrinsics \param[in] volume_size
 * size of volume in mm \param[in] Rcurr_inv inverse rotation for current camera
 * pose \param[in] tcurr translation for current camera pose \param[in]
 * tranc_dist tsdf truncation distance \param[in] volume tsdf volume to be
 * updated \param[in] buffer cyclical buffer structure \param[out]
 * depthRawScaled Buffer for scaled depth along ray
 */
PCL_EXPORTS void
integrateTsdfVolume(const PtrStepSz<ushort> &depth, const Intr &intr,
                    const float3 &volume_size, const Mat33 &Rcurr_inv,
                    const float3 &tcurr, float tranc_dist,
                    PtrStep<short2> volume, const pcl::gpu::tsdf_buffer *buffer,
                    DeviceArray2D<float> &depthScaled);

/** \brief Function that clears the TSDF values. The clearing takes place from
 * the origin (in indices) to an offset in X,Y,Z values accordingly \param[in]
 * volume Pointer to TSDF volume in GPU \param[in] buffer Pointer to the buffer
 * struct that contains information about memory addresses of the tsdf volume
 * memory block, which are used for the cyclic buffer. \param[in] shiftX Offset
 * in indices that will be cleared from the TSDF volume. The clearing start from
 * buffer.OriginX and stops in OriginX + shiftX \param[in] shiftY Offset in
 * indices that will be cleared from the TSDF volume. The clearing start from
 * buffer.OriginY and stops in OriginY + shiftY \param[in] shiftZ Offset in
 * indices that will be cleared from the TSDF volume. The clearing start from
 * buffer.OriginZ and stops in OriginZ + shiftZ
 */
PCL_EXPORTS void clearTSDFSlice(PtrStep<short2> volume,
                                pcl::gpu::tsdf_buffer *buffer, int shiftX,
                                int shiftY, int shiftZ);

/** \brief Initialzied color volume
 * \param[out] color_volume color volume for initialization
 */

void initColorVolume(PtrStep<uchar4> color_volume);

/** \brief Performs integration in color volume
 * \param[in] intr Depth camera intrionsics structure
 * \param[in] tranc_dist tsdf truncation distance
 * \param[in] R_inv Inverse camera rotation
 * \param[in] t camera translation
 * \param[in] vmap Raycasted vertex map
 * \param[in] colors RGB colors for current frame
 * \param[in] volume_size volume size in meters
 * \param[in] color_volume color volume to be integrated
 * \param[in] max_weight max weight for running color average. Zero means not
 * average, one means average with prev value, etc.
 */
void updateColorVolume(const Intr &intr, float tranc_dist, const Mat33 &R_inv,
                       const float3 &t, const MapArr &vmap,
                       const PtrStepSz<uchar3> &colors,
                       const float3 &volume_size, PtrStep<uchar4> color_volume,
                       pcl::gpu::tsdf_buffer *buffer, int max_weight = 1);

void clearColorSlice(PtrStep<uchar4> color_volume,
                     pcl::gpu::tsdf_buffer *buffer, int shiftX, int shiftY,
                     int shiftZ);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Raycast and view generation
/** \brief Generation vertex and normal maps from volume for current camera pose
 * \param[in] intr camera intrinsices
 * \param[in] Rcurr current rotation
 * \param[in] tcurr current translation
 * \param[in] tranc_dist volume truncation distance
 * \param[in] volume_size volume size in mm
 * \param[in] volume tsdf volume
 * \param[in] buffer cyclical buffer structure
 * \param[out] vmap output vertex map
 * \param[out] nmap output normals map
 */
void raycast(const Intr &intr, const Mat33 &Rcurr, const float3 &tcurr,
             float tranc_dist, const float3 &volume_size,
             const PtrStep<short2> &volume, const pcl::gpu::tsdf_buffer *buffer,
             MapArr &vmap, MapArr &nmap);

/** \brief Renders 3D image of the scene
 * \param[in] vmap vertex map
 * \param[in] nmap normals map
 * \param[in] light pose of light source
 * \param[out] dst buffer where image is generated
 */
void generateImage(const MapArr &vmap, const MapArr &nmap,
                   const LightSource &light, PtrStepSz<uchar3> dst);

/** \brief Renders depth image from give pose
 * \param[in] vmap inverse camera rotation
 * \param[in] nmap camera translation
 * \param[in] light vertex map
 * \param[out] dst buffer where depth is generated
 */
void generateDepth(const Mat33 &R_inv, const float3 &t, const MapArr &vmap,
                   DepthMap &dst);

void generateNormal(const Mat33 &R_inv, const float3 &t, const MapArr &vmap,
                    const MapArr &nmap, PtrStepSz<uchar3> dst);

/** \brief Paints 3D view with color map
 * \param[in] colors rgb color frame from OpenNI
 * \param[out] dst output 3D view
 * \param[in] colors_wight weight for colors
 */
void paint3DView(const PtrStep<uchar3> &colors, PtrStepSz<uchar3> dst,
                 float colors_weight = 0.5f);

/** \brief Performs resize of vertex map to next pyramid level by averaging each
 * four points \param[in] input vertext map \param[out] output resized vertex
 * map
 */
void resizeVMap(const MapArr &input, MapArr &output);

/** \brief Performs resize of vertex map to next pyramid level by averaging each
 * four normals \param[in] input normal map \param[out] output vertex map
 */
void resizeNMap(const MapArr &input, MapArr &output);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Push data to TSDF

/** \brief Loads the values of a tsdf point cloud to the tsdf volume in GPU
 * \param[in] volume tsdf volume
 * \param[in] cloud_gpu contains the data to be pushed to the tsdf volume
 * \param[in] buffer Pointer to the buffer struct that contains information
 * about memory addresses of the tsdf volume memory block, which are used for
 * the cyclic buffer.
 */
/*PCL_EXPORTS*/ void
pushCloudAsSliceGPU(const PtrStep<short2> &volume,
                    pcl::gpu::DeviceArray<PointType> cloud_gpu,
                    const pcl::gpu::tsdf_buffer *buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cloud extraction

/** \brief Perform point cloud extraction from tsdf volume
 * \param[in] volume tsdf volume
 * \param[in] volume_size size of the volume
 * \param[out] output buffer large enought to store point cloud
 * \return number of point stored to passed buffer
 */
PCL_EXPORTS size_t extractCloud(const PtrStep<short2> &volume,
                                const pcl::gpu::tsdf_buffer *buffer,
                                const float3 &volume_size,
                                PtrSz<PointType> output);

/** \brief Perform point cloud extraction of a slice from tsdf volume
 * \param[in] volume tsdf volume on GPU
 * \param[in] volume_size size of the volume
 * \param[in] buffer Pointer to the buffer struct that contains information
 * about memory addresses of the tsdf volume memory block, which are used for
 * the cyclic buffer. \param[in] shiftX Offset in indices that will be cleared
 * from the TSDF volume. The clearing start from buffer.OriginX and stops in
 * OriginX + shiftX \param[in] shiftY Offset in indices that will be cleared
 * from the TSDF volume. The clearing start from buffer.OriginY and stops in
 * OriginY + shiftY \param[in] shiftZ Offset in indices that will be cleared
 * from the TSDF volume. The clearing start from buffer.OriginZ and stops in
 * OriginZ + shiftZ \param[out] output_xyz buffer large enought to store point
 * cloud xyz values \param[out] output_intensities buffer large enought to store
 * point cloud intensity values \return number of point stored to passed buffer
 */
PCL_EXPORTS size_t extractSliceAsCloud(const PtrStep<short2> &volume,
                                       const float3 &volume_size,
                                       const pcl::gpu::tsdf_buffer *buffer,
                                       const int shiftX, const int shiftY,
                                       const int shiftZ,
                                       PtrSz<PointType> output_xyz,
                                       PtrSz<float> output_intensities);

/** \brief Performs normals computation for given poins using tsdf volume
 * \param[in] volume tsdf volume
 * \param[in] volume_size volume size
 * \param[in] input points where normals are computed
 * \param[out] output normals. Could be float4 or float8. If for a point normal
 * can't be computed, such normal is marked as nan.
 */
template <typename NormalType>
void extractNormals(const PtrStep<short2> &volume, const float3 &volume_size,
                    const PtrSz<PointType> &input, NormalType *output);

void extractNormalsInSpace(const PtrStep<short2> &volume,
                           const pcl::gpu::tsdf_buffer *buffer,
                           const float3 &volume_size,
                           const PtrSz<PointType> &input);

/** \brief Performs colors exctraction from color volume
 * \param[in] color_volume color volume
 * \param[in] volume_size volume size
 * \param[in] points points for which color are computed
 * \param[out] colors output array with colors.
 */
void exctractColors(const PtrStep<uchar4> &color_volume,
                    const pcl::gpu::tsdf_buffer *buffer,
                    const float3 &volume_size, const PtrSz<PointType> &points,
                    uchar4 *colors);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility
struct float8 {
    float x, y, z, w, c1, c2, c3, c4;
};
struct float12 {
    float x, y, z, w, normal_x, normal_y, normal_z, n4, c1, c2, c3, c4;
};

/** \brief Conversion from SOA to AOS
 * \param[in] vmap SOA map
 * \param[out] output Array of 3D points. Can be float4 or float8.
 */
template <typename T>
void convert(const MapArr &vmap, DeviceArray2D<T> &output);

/** \brief Merges pcl::PointXYZ and pcl::Normal to PointNormal
 * \param[in] coud points cloud
 * \param[in] normals normals cloud
 * \param[out] output array of PointNomals.
 */
void mergePointNormal(const DeviceArray<float4> &cloud,
                      const DeviceArray<float8> &normals,
                      const DeviceArray<float12> &output);

/** \brief  Check for qnan (unused now)
 * \param[in] value
 */
inline bool valid_host(float value) {
    return *reinterpret_cast<int *>(&value) != 0x7fffffff; // QNAN
}

/** \brief synchronizes CUDA execution */
inline void sync() { cudaSafeCall(cudaDeviceSynchronize()); }

template <class D, class Matx> D &device_cast(Matx &matx) {
    return (*reinterpret_cast<D *>(matx.data()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Marching cubes implementation

/** \brief Binds marching cubes tables to texture references */
void bindTextures(const int *edgeBuf, const int *triBuf,
                  const int *numVertsBuf);

/** \brief Unbinds */
void unbindTextures();

/** \brief Scans tsdf volume and retrieves occuped voxes
 * \param[in] volume tsdf volume
 * \param[out] occupied_voxels buffer for occuped voxels. The function fulfills
 * first row with voxel ids and second row with number of vertextes. \return
 * number of voxels in the buffer
 */
int getOccupiedVoxels(const PtrStep<short2> &volume,
                      DeviceArray2D<int> &occupied_voxels);

/** \brief Computes total number of vertexes for all voxels and offsets of
 * vertexes in final triangle array \param[out] occupied_voxels buffer with
 * occuped voxels. The function fulfills 3nd only with offsets \return total
 * number of vertexes
 */
int computeOffsetsAndTotalVertexes(DeviceArray2D<int> &occupied_voxels);

/** \brief Generates final triangle array
 * \param[in] volume tsdf volume
 * \param[in] occupied_voxels occuped voxel ids (first row), number of
 * vertexes(second row), offsets(third row). \param[in] volume_size volume size
 * in meters \param[out] output triangle array
 */
void generateTriangles(const PtrStep<short2> &volume,
                       const DeviceArray2D<int> &occupied_voxels,
                       const float3 &volume_size,
                       DeviceArray<PointType> &output);
} // namespace device
} // namespace pcl

#endif /* PCL_KINFU_INTERNAL_HPP_ */
