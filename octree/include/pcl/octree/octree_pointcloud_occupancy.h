/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2011, Willow Garage, Inc.
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
 * $Id: octree_pointcloud_occupancy.h 7148 2012-09-14 23:08:00Z jkammerl $
 */

#ifndef PCL_OCTREE_OCCUPANCY_H
#define PCL_OCTREE_OCCUPANCY_H

#include "octree_pointcloud.h"

#include "octree_base.h"
#include "octree2buf_base.h"

namespace pcl {
namespace octree {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief @b Octree pointcloud occupancy class
 *  \note This pointcloud octree class generate an octrees from a point cloud
 * (zero-copy). No information is stored at the lead nodes. It can be used of
 * occupancy checks. \note The octree pointcloud is initialized with its voxel
 * resolution. Its bounding box is automatically adjusted or can be predefined.
 *  \note
 *  \note typename: PointT: type of point used in pointcloud
 *  \ingroup octree
 *  \author Julius Kammerl (julius@kammerl.de)
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename LeafContainerT = OctreeContainerEmpty<int>,
          typename BranchContainerT = OctreeContainerEmpty<int>>
class OctreePointCloudOccupancy
    : public OctreePointCloud<PointT, LeafContainerT, BranchContainerT,
                              OctreeBase<int, LeafContainerT, BranchContainerT>>

{

  public:
    // public typedefs for single/double buffering
    typedef OctreePointCloudOccupancy<PointT, LeafContainerT, BranchContainerT>
        SingleBuffer;
    typedef OctreePointCloudOccupancy<PointT, LeafContainerT, BranchContainerT>
        DoubleBuffer;

    // public point cloud typedefs
    typedef typename OctreePointCloud<PointT, LeafContainerT,
                                      BranchContainerT>::PointCloud PointCloud;
    typedef typename OctreePointCloud<
        PointT, LeafContainerT, BranchContainerT>::PointCloudPtr PointCloudPtr;
    typedef typename OctreePointCloud<PointT, LeafContainerT,
                                      BranchContainerT>::PointCloudConstPtr
        PointCloudConstPtr;

    /** \brief Constructor.
     *  \param resolution_arg:  octree resolution at lowest octree level
     * */
    OctreePointCloudOccupancy(const double resolution_arg)
        : OctreePointCloud<PointT, LeafContainerT, BranchContainerT,
                           OctreeBase<int, LeafContainerT, BranchContainerT>>(
              resolution_arg) {}

    /** \brief Empty class constructor. */
    virtual ~OctreePointCloudOccupancy() {}

    /** \brief Set occupied voxel at point.
     *  \param point_arg:  input point
     * */
    void setOccupiedVoxelAtPoint(const PointT &point_arg) {
        OctreeKey key;

        // make sure bounding box is big enough
        adoptBoundingBoxToPoint(point_arg);

        // generate key
        genOctreeKeyforPoint(point_arg, key);

        // add point to octree at key
        this->addData(key, 0);
    }

    /** \brief Set occupied voxels at all points from point cloud.
     *  \param cloud_arg:  input point cloud
     * */
    void setOccupiedVoxelsAtPointsFromCloud(PointCloudPtr cloud_arg) {
        size_t i;

        for (i = 0; i < cloud_arg->points.size(); i++) {
            // check for NaNs
            if (isFinite(cloud_arg->points[i])) {
                // set voxel at point
                this->setOccupiedVoxelAtPoint(cloud_arg->points[i]);
            }
        }
    }
};
} // namespace octree

} // namespace pcl

#define PCL_INSTANTIATE_OctreePointCloudOccupancy(T)                           \
    template class PCL_EXPORTS pcl::octree::OctreePointCloudOccupancy<T>;

#endif
