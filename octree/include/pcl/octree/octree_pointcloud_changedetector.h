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
 * $Id: octree_pointcloud_changedetector.h 7148 2012-09-14 23:08:00Z jkammerl $
 */

#ifndef PCL_OCTREE_CHANGEDETECTOR_H
#define PCL_OCTREE_CHANGEDETECTOR_H

#include "octree_pointcloud.h"

#include "octree_base.h"
#include "octree2buf_base.h"

namespace pcl {
namespace octree {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief @b Octree pointcloud change detector class
 *  \note This pointcloud octree class generate an octrees from a point cloud
 * (zero-copy). It allows to detect new leaf nodes and serialize their point
 * indices \note The octree pointcloud is initialized with its voxel resolution.
 * Its bounding box is automatically adjusted or can be predefined. \note \note
 * typename: PointT: type of point used in pointcloud \ingroup octree \author
 * Julius Kammerl (julius@kammerl.de)
 */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT,
          typename LeafContainerT = OctreeContainerDataTVector<int>,
          typename BranchContainerT = OctreeContainerEmpty<int>>

class OctreePointCloudChangeDetector
    : public OctreePointCloud<
          PointT, LeafContainerT, BranchContainerT,
          Octree2BufBase<int, LeafContainerT, BranchContainerT>>

{

  public:
    /** \brief Constructor.
     *  \param resolution_arg:  octree resolution at lowest octree level
     * */
    OctreePointCloudChangeDetector(const double resolution_arg)
        : OctreePointCloud<
              PointT, LeafContainerT, BranchContainerT,
              Octree2BufBase<int, LeafContainerT, BranchContainerT>>(
              resolution_arg) {}

    /** \brief Empty class constructor. */
    virtual ~OctreePointCloudChangeDetector() {}

    /** \brief Get a indices from all leaf nodes that did not exist in previous
     * buffer. \param indicesVector_arg: results are written to this vector of
     * int indices \param minPointsPerLeaf_arg: minimum amount of points
     * required within leaf node to become serialized. \return number of point
     * indices
     */
    int getPointIndicesFromNewVoxels(std::vector<int> &indicesVector_arg,
                                     const int minPointsPerLeaf_arg = 0) {
        this->serializeNewLeafs(indicesVector_arg, minPointsPerLeaf_arg);
        return (static_cast<int>(indicesVector_arg.size()));
    }
};
} // namespace octree
} // namespace pcl

#define PCL_INSTANTIATE_OctreePointCloudChangeDetector(T)                      \
    template class PCL_EXPORTS pcl::octree::OctreePointCloudChangeDetector<T>;

#endif
