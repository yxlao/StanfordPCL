/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
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
 * $Id$
 */

#ifndef PCL_FEATURES_IMPL_SHOT_LRF_OMP_H_
#define PCL_FEATURES_IMPL_SHOT_LRF_OMP_H_

#include <utility>
#include <pcl/features/shot_lrf_omp.h>
#include <pcl/features/shot_lrf.h>

template <typename PointInT, typename PointOutT>
void pcl::SHOTLocalReferenceFrameEstimationOMP<
    PointInT, PointOutT>::computeFeature(PointCloudOut &output) {
    // check whether used with search radius or search k-neighbors
    if (this->getKSearch() != 0) {
        PCL_ERROR("[pcl::%s::computeFeature] Error! Search method set to "
                  "k-neighborhood. Call setKSearch(0) and setRadiusSearch( "
                  "radius ) to use this class.\n",
                  getClassName().c_str());
        return;
    }
    tree_->setSortedResults(true);

    int data_size = static_cast<int>(indices_->size());
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads_)
#endif
    for (int i = 0; i < data_size; ++i) {
        // point result
        Eigen::Matrix3f rf;
        PointOutT &output_rf = output[i];

        // output_rf.confidence = getLocalRF ((*indices_)[i], rf);
        // if (output_rf.confidence == std::numeric_limits<float>::max ())

        std::vector<int> n_indices;
        std::vector<float> n_sqr_distances;
        this->searchForNeighbors((*indices_)[i], search_parameter_, n_indices,
                                 n_sqr_distances);
        if (getLocalRF((*indices_)[i], rf) ==
            std::numeric_limits<float>::max()) {
            output.is_dense = false;
        }

        for (int d = 0; d < 3; ++d) {
            output_rf.x_axis[d] = rf.row(0)[d];
            output_rf.y_axis[d] = rf.row(1)[d];
            output_rf.z_axis[d] = rf.row(2)[d];
        }
    }
}

template <typename PointInT, typename PointOutT>
void pcl::SHOTLocalReferenceFrameEstimationOMP<PointInT, PointOutT>::
    computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &output) {
    // check whether used with search radius or search k-neighbors
    if (this->getKSearch() != 0) {
        PCL_ERROR("[pcl::%s::computeFeatureEigen] Error! Search method set to "
                  "k-neighborhood. Call setKSearch(0) and setRadiusSearch( "
                  "radius ) to use this class.\n",
                  getClassName().c_str());
        return;
    }
    tree_->setSortedResults(true);

    int data_size = static_cast<int>(indices_->size());

    // Set up the output channels
    output.channels["shot_lrf"].name = "shot_lrf";
    output.channels["shot_lrf"].offset = 0;
    output.channels["shot_lrf"].size = 4;
    output.channels["shot_lrf"].count = 9;
    output.channels["shot_lrf"].datatype = sensor_msgs::PointField::FLOAT32;

    // output.points.resize (indices_->size (), 10);
    output.points.resize(data_size, 9);

#ifdef _OPENMP
#pragma omp parallel for num_threads(threads_)
#endif
    for (int i = 0; i < data_size; ++i) {
        // point result
        Eigen::Matrix3f rf;

        // output.points (i, 9) = getLocalRF ((*indices_)[i], rf);
        // if (output.points (i, 9) == std::numeric_limits<float>::max ())
        if (getLocalRF((*indices_)[i], rf) ==
            std::numeric_limits<float>::max()) {
            output.is_dense = false;
        }

        output.points.block<1, 3>(i, 0).matrix() = rf.row(0);
        output.points.block<1, 3>(i, 3).matrix() = rf.row(1);
        output.points.block<1, 3>(i, 6).matrix() = rf.row(2);
    }
}

#define PCL_INSTANTIATE_SHOTLocalReferenceFrameEstimationOMP(T, OutT)          \
    template class PCL_EXPORTS                                                 \
        pcl::SHOTLocalReferenceFrameEstimationOMP<T, OutT>;

#endif // PCL_FEATURES_IMPL_SHOT_LRF_H_
