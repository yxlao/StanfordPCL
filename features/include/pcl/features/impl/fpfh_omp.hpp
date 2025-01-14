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
 * $Id: fpfh_omp.hpp 6968 2012-08-26 18:46:25Z jspricke $
 *
 */

#ifndef PCL_FEATURES_IMPL_FPFH_OMP_H_
#define PCL_FEATURES_IMPL_FPFH_OMP_H_

#include <pcl/features/fpfh_omp.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointNT, typename PointOutT>
void pcl::FPFHEstimationOMP<PointInT, PointNT, PointOutT>::computeFeature(
    PointCloudOut &output) {
    std::vector<int> spfh_indices_vec;
    std::vector<int> spfh_hist_lookup(surface_->points.size());

    // Build a list of (unique) indices for which we will need to compute SPFH
    // signatures (We need an SPFH signature for every point that is a neighbor
    // of any point in input_[indices_])
    if (surface_ != input_ || indices_->size() != surface_->points.size()) {
        std::vector<int> nn_indices(
            k_); // \note These resizes are irrelevant for a radiusSearch ().
        std::vector<float> nn_dists(k_);

        std::set<int> spfh_indices_set;
        for (size_t idx = 0; idx < indices_->size(); ++idx) {
            int p_idx = (*indices_)[idx];
            if (this->searchForNeighbors(p_idx, search_parameter_, nn_indices,
                                         nn_dists) == 0)
                continue;

            spfh_indices_set.insert(nn_indices.begin(), nn_indices.end());
        }
        spfh_indices_vec.resize(spfh_indices_set.size());
        std::copy(spfh_indices_set.begin(), spfh_indices_set.end(),
                  spfh_indices_vec.begin());
    } else {
        // Special case: When a feature must be computed at every point, there
        // is no need for a neighborhood search
        spfh_indices_vec.resize(indices_->size());
        for (int idx = 0; idx < static_cast<int>(indices_->size()); ++idx)
            spfh_indices_vec[idx] = idx;
    }

    // Initialize the arrays that will store the SPFH signatures
    size_t data_size = spfh_indices_vec.size();
    hist_f1_.setZero(data_size, nr_bins_f1_);
    hist_f2_.setZero(data_size, nr_bins_f2_);
    hist_f3_.setZero(data_size, nr_bins_f3_);

    std::vector<int> nn_indices(
        k_); // \note These resizes are irrelevant for a radiusSearch ().
    std::vector<float> nn_dists(k_);

    // Compute SPFH signatures for every point that needs them

#ifdef _OPENMP
#pragma omp parallel for shared(spfh_hist_lookup) private(                     \
    nn_indices, nn_dists) num_threads(threads_)
#endif
    for (int i = 0; i < static_cast<int>(spfh_indices_vec.size()); ++i) {
        // Get the next point index
        int p_idx = spfh_indices_vec[i];

        // Find the neighborhood around p_idx
        if (this->searchForNeighbors(*surface_, p_idx, search_parameter_,
                                     nn_indices, nn_dists) == 0)
            continue;

        // Estimate the SPFH signature around p_idx
        this->computePointSPFHSignature(*surface_, *normals_, p_idx, i,
                                        nn_indices, hist_f1_, hist_f2_,
                                        hist_f3_);

        // Populate a lookup table for converting a point index to its
        // corresponding row in the spfh_hist_* matrices
        spfh_hist_lookup[p_idx] = i;
    }

    // Intialize the array that will store the FPFH signature
    int nr_bins = nr_bins_f1_ + nr_bins_f2_ + nr_bins_f3_;

    nn_indices.clear();
    nn_dists.clear();

    // Iterate over the entire index vector
#ifdef _OPENMP
#pragma omp parallel for shared(output) private(nn_indices, nn_dists)          \
    num_threads(threads_)
#endif
    for (int idx = 0; idx < static_cast<int>(indices_->size()); ++idx) {
        // Find the indices of point idx's neighbors...
        if (!isFinite((*input_)[(*indices_)[idx]]) ||
            this->searchForNeighbors((*indices_)[idx], search_parameter_,
                                     nn_indices, nn_dists) == 0) {
            for (int d = 0; d < nr_bins; ++d)
                output.points[idx].histogram[d] =
                    std::numeric_limits<float>::quiet_NaN();

            output.is_dense = false;
            continue;
        }

        // ... and remap the nn_indices values so that they represent row
        // indices in the spfh_hist_* matrices instead of indices into
        // surface_->points
        for (size_t i = 0; i < nn_indices.size(); ++i)
            nn_indices[i] = spfh_hist_lookup[nn_indices[i]];

        // Compute the FPFH signature (i.e. compute a weighted combination of
        // local SPFH signatures) ...
        Eigen::VectorXf fpfh_histogram = Eigen::VectorXf::Zero(nr_bins);
        weightPointSPFHSignature(hist_f1_, hist_f2_, hist_f3_, nn_indices,
                                 nn_dists, fpfh_histogram);

        // ...and copy it into the output cloud
        for (int d = 0; d < nr_bins; ++d)
            output.points[idx].histogram[d] = fpfh_histogram[d];
    }
}

#define PCL_INSTANTIATE_FPFHEstimationOMP(T, NT, OutT)                         \
    template class PCL_EXPORTS pcl::FPFHEstimationOMP<T, NT, OutT>;

#endif // PCL_FEATURES_IMPL_FPFH_OMP_H_
