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
 */

#ifndef PCL_FEATURES_LINEAR_LEAST_SQUARES_NORMAL_H_
#define PCL_FEATURES_LINEAR_LEAST_SQUARES_NORMAL_H_

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/features/feature.h>

namespace pcl {
/**
 * \brief Surface normal estimation on dense data using a least-squares
 * estimation based on a first-order Taylor approximation. \author Stefan
 * Holzer, Cedric Cagniart
 */
template <typename PointInT, typename PointOutT>
class LinearLeastSquaresNormalEstimation : public Feature<PointInT, PointOutT> {

    using Feature<PointInT, PointOutT>::input_;
    using Feature<PointInT, PointOutT>::feature_name_;
    using Feature<PointInT, PointOutT>::tree_;
    using Feature<PointInT, PointOutT>::k_;

  public:
    typedef typename Feature<PointInT, PointOutT>::PointCloudIn PointCloudIn;
    typedef typename Feature<PointInT, PointOutT>::PointCloudOut PointCloudOut;

    /** \brief Constructor */
    LinearLeastSquaresNormalEstimation()
        : use_depth_dependent_smoothing_(false), max_depth_change_factor_(1.0f),
          normal_smoothing_size_(9.0f) {
        feature_name_ = "LinearLeastSquaresNormalEstimation";
        tree_.reset();
        k_ = 1;
    };

    /** \brief Destructor */
    virtual ~LinearLeastSquaresNormalEstimation();

    /** \brief Computes the normal at the specified position.
     * \param pos_x x position (pixel)
     * \param pos_y y position (pixel)
     * \param normal the output estimated normal
     */
    void computePointNormal(const int pos_x, const int pos_y,
                            PointOutT &normal);

    /** \brief Set the normal smoothing size
     * \param normal_smoothing_size factor which influences the size of the area
     * used to smooth normals (depth dependent if useDepthDependentSmoothing is
     * true)
     */
    void setNormalSmoothingSize(float normal_smoothing_size) {
        normal_smoothing_size_ = normal_smoothing_size;
    }

    /** \brief Set whether to use depth depending smoothing or not
     * \param use_depth_dependent_smoothing decides whether the smoothing is
     * depth dependent
     */
    void setDepthDependentSmoothing(bool use_depth_dependent_smoothing) {
        use_depth_dependent_smoothing_ = use_depth_dependent_smoothing;
    }

    /** \brief The depth change threshold for computing object borders
     * \param max_depth_change_factor the depth change threshold for computing
     * object borders based on depth changes
     */
    void setMaxDepthChangeFactor(float max_depth_change_factor) {
        max_depth_change_factor_ = max_depth_change_factor;
    }

    /** \brief Provide a pointer to the input dataset (overwrites the
     * PCLBase::setInputCloud method) \param cloud the const boost shared
     * pointer to a PointCloud message
     */
    virtual inline void
    setInputCloud(const typename PointCloudIn::ConstPtr &cloud) {
        input_ = cloud;
    }

  protected:
    /** \brief Computes the normal for the complete cloud.
     * \param output the resultant normals
     */
    void computeFeature(PointCloudOut &output);

  private:
    /** the threshold used to detect depth discontinuities */
    // float distance_threshold_;

    /** \brief Smooth data based on depth (true/false). */
    bool use_depth_dependent_smoothing_;

    /** \brief Threshold for detecting depth discontinuities */
    float max_depth_change_factor_;

    /** \brief */
    float normal_smoothing_size_;

    /** \brief Make the computeFeature (&Eigen::MatrixXf); inaccessible from
     * outside the class \param[out] output the output point cloud
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &) {}
};
} // namespace pcl

#endif
