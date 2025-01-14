/*
 * Software License Agreement (BSD License)
 *
 * Point Cloud Library (PCL) - www.pointclouds.org
 * Copyright (c) 2009-2012, Willow Garage, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 * * Neither the name of Willow Garage, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 *
 */

#ifndef PCL_SURFACE_IMPL_BILATERAL_UPSAMPLING_H_
#define PCL_SURFACE_IMPL_BILATERAL_UPSAMPLING_H_

#include <pcl/surface/bilateral_upsampling.h>
#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT>
void pcl::BilateralUpsampling<PointInT, PointOutT>::process(
    pcl::PointCloud<PointOutT> &output) {
    // Copy the header
    output.header = input_->header;

    if (!initCompute()) {
        output.width = output.height = 0;
        output.points.clear();
        return;
    }

    if (input_->isOrganized() == false) {
        PCL_ERROR("Input cloud is not organized.\n");
        return;
    }

    // Invert projection matrix
    unprojection_matrix_ = projection_matrix_.inverse();

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            printf("%f ", unprojection_matrix_(i, j));

        printf("\n");
    }

    // Perform the actual surface reconstruction
    performProcessing(output);

    deinitCompute();
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT>
void pcl::BilateralUpsampling<PointInT, PointOutT>::performProcessing(
    PointCloudOut &output) {
    output.resize(input_->size());
    float nan = std::numeric_limits<float>::quiet_NaN();

    for (int x = 0; x < static_cast<int>(input_->width); ++x)
        for (int y = 0; y < static_cast<int>(input_->height); ++y) {
            int start_window_x = std::max(x - window_size_, 0),
                start_window_y = std::max(y - window_size_, 0),
                end_window_x =
                    std::min(x + window_size_, static_cast<int>(input_->width)),
                end_window_y = std::min(y + window_size_,
                                        static_cast<int>(input_->height));

            float sum = 0.0f, norm_sum = 0.0f;

            for (int x_w = start_window_x; x_w < end_window_x; ++x_w)
                for (int y_w = start_window_y; y_w < end_window_y; ++y_w) {
                    float dx = float(x - x_w), dy = float(y - y_w);

                    float val_exp_depth =
                        expf(-(dx * dx + dy * dy) /
                             (2.0f *
                              static_cast<float>(sigma_depth_ * sigma_depth_)));

                    float d_color = static_cast<float>(
                        abs(input_->points[y_w * input_->width + x_w].r -
                            input_->points[y * input_->width + x].r) +
                        abs(input_->points[y_w * input_->width + x_w].g -
                            input_->points[y * input_->width + x].g) +
                        abs(input_->points[y_w * input_->width + x_w].b -
                            input_->points[y * input_->width + x].b));
                    float val_exp_rgb =
                        expf(-d_color * d_color /
                             (2.0f * sigma_color_ * sigma_color_));

                    if (pcl_isfinite(
                            input_->points[y_w * input_->width + x_w].z)) {
                        sum += val_exp_depth * val_exp_rgb *
                               input_->points[y_w * input_->width + x_w].z;
                        norm_sum += val_exp_depth * val_exp_rgb;
                    }
                }

            output.points[y * input_->width + x].r =
                input_->points[y * input_->width + x].r;
            output.points[y * input_->width + x].g =
                input_->points[y * input_->width + x].g;
            output.points[y * input_->width + x].b =
                input_->points[y * input_->width + x].b;

            if (norm_sum != 0.0f) {
                float depth = sum / norm_sum;
                Eigen::Vector3f pc(static_cast<float>(x) * depth,
                                   static_cast<float>(y) * depth, depth);
                Eigen::Vector3f pw(unprojection_matrix_ * pc);
                output.points[y * input_->width + x].x = pw[0];
                output.points[y * input_->width + x].y = pw[1];
                output.points[y * input_->width + x].z = pw[2];
            } else {
                output.points[y * input_->width + x].x = nan;
                output.points[y * input_->width + x].y = nan;
                output.points[y * input_->width + x].z = nan;
            }
        }

    output.header = input_->header;
    output.width = input_->width;
    output.height = input_->height;
}

#define PCL_INSTANTIATE_BilateralUpsampling(T, OutT)                           \
    template class PCL_EXPORTS pcl::BilateralUpsampling<T, OutT>;

#endif /* PCL_SURFACE_IMPL_BILATERAL_UPSAMPLING_H_ */
