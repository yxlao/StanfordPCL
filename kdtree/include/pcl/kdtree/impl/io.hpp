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
 * $Id: io.hpp 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_KDTREE_IO_IMPL_HPP_
#define PCL_KDTREE_IO_IMPL_HPP_

#include <pcl/kdtree/io.h>
#include <pcl/kdtree/kdtree_flann.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename Point1T, typename Point2T>
void pcl::getApproximateIndices(
    const typename pcl::PointCloud<Point1T>::Ptr &cloud_in,
    const typename pcl::PointCloud<Point2T>::Ptr &cloud_ref,
    std::vector<int> &indices) {
    pcl::KdTreeFLANN<Point2T> tree;
    tree.setInputCloud(cloud_ref);

    std::vector<int> nn_idx(1);
    std::vector<float> nn_dists(1);
    indices.resize(cloud_in->points.size());
    for (size_t i = 0; i < cloud_in->points.size(); ++i) {
        tree.nearestKSearch(*cloud_in, i, 1, nn_idx, nn_dists);
        indices[i] = nn_idx[0];
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::getApproximateIndices(
    const typename pcl::PointCloud<PointT>::Ptr &cloud_in,
    const typename pcl::PointCloud<PointT>::Ptr &cloud_ref,
    std::vector<int> &indices) {
    pcl::KdTreeFLANN<PointT> tree;
    tree.setInputCloud(cloud_ref);

    std::vector<int> nn_idx(1);
    std::vector<float> nn_dists(1);
    indices.resize(cloud_in->points.size());
    for (size_t i = 0; i < cloud_in->points.size(); ++i) {
        tree.nearestKSearch(*cloud_in, i, 1, nn_idx, nn_dists);
        indices[i] = nn_idx[0];
    }
}

#endif // PCL_KDTREE_IO_IMPL_H_
