/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2009, Willow Garage, Inc.
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
 * $Id: extract_clusters.hpp 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_SEGMENTATION_IMPL_EXTRACT_CLUSTERS_H_
#define PCL_SEGMENTATION_IMPL_EXTRACT_CLUSTERS_H_

#include <pcl/segmentation/extract_clusters.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::extractEuclideanClusters(
    const PointCloud<PointT> &cloud,
    const boost::shared_ptr<search::Search<PointT>> &tree, float tolerance,
    std::vector<PointIndices> &clusters, unsigned int min_pts_per_cluster,
    unsigned int max_pts_per_cluster) {
    if (tree->getInputCloud()->points.size() != cloud.points.size()) {
        PCL_ERROR("[pcl::extractEuclideanClusters] Tree built for a different "
                  "point cloud dataset (%zu) than the input cloud (%zu)!\n",
                  tree->getInputCloud()->points.size(), cloud.points.size());
        return;
    }
    // Create a bool vector of processed point indices, and initialize it to
    // false
    std::vector<bool> processed(cloud.points.size(), false);

    std::vector<int> nn_indices;
    std::vector<float> nn_distances;
    // Process all points in the indices vector
    for (int i = 0; i < static_cast<int>(cloud.points.size()); ++i) {
        if (processed[i])
            continue;

        std::vector<int> seed_queue;
        int sq_idx = 0;
        seed_queue.push_back(i);

        processed[i] = true;

        while (sq_idx < static_cast<int>(seed_queue.size())) {
            // Search for sq_idx
            if (!tree->radiusSearch(seed_queue[sq_idx], tolerance, nn_indices,
                                    nn_distances)) {
                sq_idx++;
                continue;
            }

            for (size_t j = 1; j < nn_indices.size();
                 ++j) // nn_indices[0] should be sq_idx
            {
                if (nn_indices[j] == -1 ||
                    processed[nn_indices[j]]) // Has this point been processed
                                              // before ?
                    continue;

                // Perform a simple Euclidean clustering
                seed_queue.push_back(nn_indices[j]);
                processed[nn_indices[j]] = true;
            }

            sq_idx++;
        }

        // If this queue is satisfactory, add to the clusters
        if (seed_queue.size() >= min_pts_per_cluster &&
            seed_queue.size() <= max_pts_per_cluster) {
            pcl::PointIndices r;
            r.indices.resize(seed_queue.size());
            for (size_t j = 0; j < seed_queue.size(); ++j)
                r.indices[j] = seed_queue[j];

            // These two lines should not be needed: (can anyone confirm?) -FF
            std::sort(r.indices.begin(), r.indices.end());
            r.indices.erase(std::unique(r.indices.begin(), r.indices.end()),
                            r.indices.end());

            r.header = cloud.header;
            clusters.push_back(
                r); // We could avoid a copy by working directly in the vector
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
/** @todo: fix the return value, make sure the exit is not needed anymore*/
template <typename PointT>
void pcl::extractEuclideanClusters(
    const PointCloud<PointT> &cloud, const std::vector<int> &indices,
    const boost::shared_ptr<search::Search<PointT>> &tree, float tolerance,
    std::vector<PointIndices> &clusters, unsigned int min_pts_per_cluster,
    unsigned int max_pts_per_cluster) {
    // \note If the tree was created over <cloud, indices>, we guarantee a 1-1
    // mapping between what the tree returns
    // and indices[i]
    if (tree->getInputCloud()->points.size() != cloud.points.size()) {
        PCL_ERROR("[pcl::extractEuclideanClusters] Tree built for a different "
                  "point cloud dataset (%zu) than the input cloud (%zu)!\n",
                  tree->getInputCloud()->points.size(), cloud.points.size());
        return;
    }
    if (tree->getIndices()->size() != indices.size()) {
        PCL_ERROR("[pcl::extractEuclideanClusters] Tree built for a different "
                  "set of indices (%zu) than the input set (%zu)!\n",
                  tree->getIndices()->size(), indices.size());
        return;
    }

    // Create a bool vector of processed point indices, and initialize it to
    // false
    std::vector<bool> processed(cloud.points.size(), false);

    std::vector<int> nn_indices;
    std::vector<float> nn_distances;
    // Process all points in the indices vector
    for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
        if (processed[indices[i]])
            continue;

        std::vector<int> seed_queue;
        int sq_idx = 0;
        seed_queue.push_back(indices[i]);

        processed[indices[i]] = true;

        while (sq_idx < static_cast<int>(seed_queue.size())) {
            // Search for sq_idx
            int ret = tree->radiusSearch(cloud.points[seed_queue[sq_idx]],
                                         tolerance, nn_indices, nn_distances);
            if (ret == -1) {
                PCL_ERROR("[pcl::extractEuclideanClusters] Received error code "
                          "-1 from radiusSearch\n");
                exit(0);
            }
            if (!ret) {
                sq_idx++;
                continue;
            }

            for (size_t j = 1; j < nn_indices.size();
                 ++j) // nn_indices[0] should be sq_idx
            {
                if (nn_indices[j] == -1 ||
                    processed[nn_indices[j]]) // Has this point been processed
                                              // before ?
                    continue;

                // Perform a simple Euclidean clustering
                seed_queue.push_back(nn_indices[j]);
                processed[nn_indices[j]] = true;
            }

            sq_idx++;
        }

        // If this queue is satisfactory, add to the clusters
        if (seed_queue.size() >= min_pts_per_cluster &&
            seed_queue.size() <= max_pts_per_cluster) {
            pcl::PointIndices r;
            r.indices.resize(seed_queue.size());
            for (size_t j = 0; j < seed_queue.size(); ++j)
                // This is the only place where indices come into play
                r.indices[j] = seed_queue[j];

            // These two lines should not be needed: (can anyone confirm?) -FF
            // r.indices.assign(seed_queue.begin(), seed_queue.end());
            std::sort(r.indices.begin(), r.indices.end());
            r.indices.erase(std::unique(r.indices.begin(), r.indices.end()),
                            r.indices.end());

            r.header = cloud.header;
            clusters.push_back(
                r); // We could avoid a copy by working directly in the vector
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template <typename PointT>
void pcl::EuclideanClusterExtraction<PointT>::extract(
    std::vector<PointIndices> &clusters) {
    if (!initCompute() || (input_ != 0 && input_->points.empty()) ||
        (indices_ != 0 && indices_->empty())) {
        clusters.clear();
        return;
    }

    // Initialize the spatial locator
    if (!tree_) {
        if (input_->isOrganized())
            tree_.reset(new pcl::search::OrganizedNeighbor<PointT>());
        else
            tree_.reset(new pcl::search::KdTree<PointT>(false));
    }

    // Send the input dataset to the spatial locator
    tree_->setInputCloud(input_, indices_);
    extractEuclideanClusters(*input_, *indices_, tree_,
                             static_cast<float>(cluster_tolerance_), clusters,
                             min_pts_per_cluster_, max_pts_per_cluster_);

    // tree_->setInputCloud (input_);
    // extractEuclideanClusters (*input_, tree_, cluster_tolerance_, clusters,
    // min_pts_per_cluster_, max_pts_per_cluster_);

    // Sort the clusters based on their size (largest one first)
    std::sort(clusters.rbegin(), clusters.rend(), comparePointClusters);

    deinitCompute();
}

#define PCL_INSTANTIATE_EuclideanClusterExtraction(T)                          \
    template class PCL_EXPORTS pcl::EuclideanClusterExtraction<T>;
#define PCL_INSTANTIATE_extractEuclideanClusters(T)                            \
    template void PCL_EXPORTS pcl::extractEuclideanClusters<T>(                \
        const pcl::PointCloud<T> &,                                            \
        const boost::shared_ptr<pcl::search::Search<T>> &, float,              \
        std::vector<pcl::PointIndices> &, unsigned int, unsigned int);
#define PCL_INSTANTIATE_extractEuclideanClusters_indices(T)                    \
    template void PCL_EXPORTS pcl::extractEuclideanClusters<T>(                \
        const pcl::PointCloud<T> &, const std::vector<int> &,                  \
        const boost::shared_ptr<pcl::search::Search<T>> &, float,              \
        std::vector<pcl::PointIndices> &, unsigned int, unsigned int);

#endif // PCL_EXTRACT_CLUSTERS_IMPL_H_
