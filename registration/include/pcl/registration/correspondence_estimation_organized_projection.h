/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2012-, Open Perception, Inc.
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
 *   * Neither the name of the copyright holder(s) nor the names of its
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
 * $Id: correspondence_estimation_organized_projection.h 7179 2012-09-18
 * 00:36:23Z rusu $
 *
 */

#ifndef PCL_REGISTRATION_CORRESPONDENCE_ESTIMATION_ORGANIZED_PROJECTION_H_
#define PCL_REGISTRATION_CORRESPONDENCE_ESTIMATION_ORGANIZED_PROJECTION_H_

#include <pcl/registration/correspondence_estimation.h>

namespace pcl {
namespace registration {
/** \brief CorrespondenceEstimationOrganizedProjection computes correspondences
 * by projecting the source point cloud onto the target point cloud using the
 * camera intrinsic and extrinsic parameters. The correspondences can be trimmed
 * by a depth threshold and by a distance threshold.
 * It is not as precise as a nearest neighbor search, but it is much faster, as
 * it avoids the usage of any additional structures (i.e., kd-trees). \Note The
 * target point cloud must be organized (no restrictions on the source) and the
 * target point cloud must be given in the camera coordinate frame. Any other
 * transformation is specified by the src_to_tgt_transformation_ variable.
 * \author Alex Ichim
 */
template <typename PointSource, typename PointTarget>
class CorrespondenceEstimationOrganizedProjection
    : public CorrespondenceEstimationBase<PointSource, PointTarget> {
  public:
    using CorrespondenceEstimationBase<PointSource, PointTarget>::initCompute;
    using PCLBase<PointSource>::deinitCompute;
    using PCLBase<PointSource>::input_;
    using PCLBase<PointSource>::indices_;
    using CorrespondenceEstimationBase<PointSource, PointTarget>::getClassName;
    using CorrespondenceEstimationBase<PointSource,
                                       PointTarget>::point_representation_;

    typedef pcl::PointCloud<PointSource> PointCloudSource;
    typedef typename PointCloudSource::Ptr PointCloudSourcePtr;
    typedef typename PointCloudSource::ConstPtr PointCloudSourceConstPtr;

    typedef pcl::PointCloud<PointTarget> PointCloudTarget;
    typedef typename PointCloudTarget::Ptr PointCloudTargetPtr;
    typedef typename PointCloudTarget::ConstPtr PointCloudTargetConstPtr;

    /** \brief Empty constructor that sets all the intrinsic calibration to the
     * default Kinect values. */
    CorrespondenceEstimationOrganizedProjection()
        : fx_(525.f), fy_(525.f), cx_(320.f), cy_(240.f),
          src_to_tgt_transformation_(Eigen::Matrix4f::Identity()),
          depth_threshold_(std::numeric_limits<float>::max()),
          projection_matrix_(Eigen::Matrix3f::Identity()) {}

    /** \brief Sets the focal length parameters of the target camera.
     * \param[in] fx the focal length in pixels along the x-axis of the image
     * \param[in] fy the focal length in pixels along the y-axis of the image
     */
    inline void setFocalLengths(const float fx, const float fy) {
        fx_ = fx;
        fy_ = fy;
    }

    /** \brief Reads back the focal length parameters of the target camera.
     * \param[out] fx the focal length in pixels along the x-axis of the image
     * \param[out] fy the focal length in pixels along the y-axis of the image
     */
    inline void getFocalLengths(float &fx, float &fy) const {
        fx = fx_;
        fy = fy_;
    }

    /** \brief Sets the camera center parameters of the target camera.
     * \param[in] cx the x-coordinate of the camera center
     * \param[in] cy the y-coordinate of the camera center
     */
    inline void setCameraCenters(const float cx, const float cy) {
        cx_ = cx;
        cy_ = cy;
    }

    /** \brief Reads back the camera center parameters of the target camera.
     * \param[out] cx the x-coordinate of the camera center
     * \param[out] cy the y-coordinate of the camera center
     */
    inline void getCameraCenters(float &cx, float &cy) const {
        cx = cx_;
        cy = cy_;
    }

    /** \brief Sets the transformation from the source point cloud to the target
     * point cloud. \Note The target point cloud must be in its local camera
     * coordinates, so use this transformation to correct for that. \param[in]
     * src_to_tgt_transformation the transformation
     */
    inline void
    setSourceTransformation(const Eigen::Matrix4f &src_to_tgt_transformation) {
        src_to_tgt_transformation_ = src_to_tgt_transformation;
    }

    /** \brief Reads back the transformation from the source point cloud to the
     * target point cloud. \Note The target point cloud must be in its local
     * camera coordinates, so use this transformation to correct for that.
     * \param[out] src_to_tgt_transformation the transformation
     */
    inline Eigen::Matrix4f getSourceTransformation() const {
        return (src_to_tgt_transformation_);
    }

    /** \brief Sets the depth threshold; after projecting the source points in
     * the image space of the target camera, this threshold is applied on the
     * depths of corresponding dexels to eliminate the ones that are too far
     * from each other. \param[in] depth_threshold the depth threshold
     */
    inline void setDepthThreshold(const float depth_threshold) {
        depth_threshold_ = depth_threshold;
    }

    /** \brief Reads back the depth threshold; after projecting the source
     * points in the image space of the target camera, this threshold is applied
     * on the depths of corresponding dexels to eliminate the ones that are too
     * far from each other.
     * \param[out] depth_threshold the depth threshold
     */
    inline float getDepthThreshold() const { return (depth_threshold_); }

    /** \brief Computes the correspondences, applying a maximum Euclidean
     * distance threshold. \param[in] max_distance Euclidean distance threshold
     * above which correspondences will be rejected
     */
    void determineCorrespondences(Correspondences &correspondences,
                                  double max_distance);

  protected:
    using CorrespondenceEstimationBase<PointSource, PointTarget>::target_;

    bool initCompute();

    float fx_, fy_;
    float cx_, cy_;
    Eigen::Matrix4f src_to_tgt_transformation_;
    float depth_threshold_;

    Eigen::Matrix3f projection_matrix_;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
} // namespace registration
} // namespace pcl

#include <pcl/registration/impl/correspondence_estimation_organized_projection.hpp>

#endif /* PCL_REGISTRATION_CORRESPONDENCE_ESTIMATION_ORGANIZED_PROJECTION_H_   \
        */
