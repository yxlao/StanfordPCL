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
 * $Id: sac_model_parallel_plane.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_SAMPLE_CONSENSUS_MODEL_PARALLELPLANE_H_
#define PCL_SAMPLE_CONSENSUS_MODEL_PARALLELPLANE_H_

#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/common/common.h>

namespace pcl {
/** \brief @b SampleConsensusModelParallelPlane defines a model for 3D plane
 * segmentation using additional angular constraints. The plane must be parallel
 * to a user-specified axis
 * (\ref setAxis) within an user-specified angle threshold (\ref setEpsAngle).
 *
 * Code example for a plane model, parallel (within a 15 degrees tolerance) with
 * the Z axis: \code SampleConsensusModelParallelPlane<pcl::PointXYZ> model
 * (cloud); model.setAxis (Eigen::Vector3f (0.0, 0.0, 1.0)); model.setEpsAngle
 * (pcl::deg2rad (15)); \endcode
 *
 * \note Please remember that you need to specify an angle > 0 in order to
 * activate the axis-angle constraint!
 *
 * \author Radu Bogdan Rusu, Nico Blodow
 * \ingroup sample_consensus
 */
template <typename PointT>
class SampleConsensusModelParallelPlane
    : public SampleConsensusModelPlane<PointT> {
  public:
    typedef typename SampleConsensusModelPlane<PointT>::PointCloud PointCloud;
    typedef
        typename SampleConsensusModelPlane<PointT>::PointCloudPtr PointCloudPtr;
    typedef typename SampleConsensusModelPlane<PointT>::PointCloudConstPtr
        PointCloudConstPtr;

    typedef boost::shared_ptr<SampleConsensusModelParallelPlane> Ptr;

    /** \brief Constructor for base SampleConsensusModelParallelPlane.
     * \param[in] cloud the input point cloud dataset
     */
    SampleConsensusModelParallelPlane(const PointCloudConstPtr &cloud)
        : SampleConsensusModelPlane<PointT>(cloud),
          axis_(Eigen::Vector3f::Zero()), eps_angle_(0.0), sin_angle_(-1.0) {}

    /** \brief Constructor for base SampleConsensusModelParallelPlane.
     * \param[in] cloud the input point cloud dataset
     * \param[in] indices a vector of point indices to be used from \a cloud
     */
    SampleConsensusModelParallelPlane(const PointCloudConstPtr &cloud,
                                      const std::vector<int> &indices)
        : SampleConsensusModelPlane<PointT>(cloud, indices),
          axis_(Eigen::Vector3f::Zero()), eps_angle_(0.0), sin_angle_(-1.0) {}

    /** \brief Set the axis along which we need to search for a plane
     * perpendicular to. \param[in] ax the axis along which we need to search
     * for a plane perpendicular to
     */
    inline void setAxis(const Eigen::Vector3f &ax) { axis_ = ax; }

    /** \brief Get the axis along which we need to search for a plane
     * perpendicular to. */
    inline Eigen::Vector3f getAxis() { return (axis_); }

    /** \brief Set the angle epsilon (delta) threshold.
     * \param[in] ea the maximum allowed difference between the plane normal and
     * the given axis. \note You need to specify an angle > 0 in order to
     * activate the axis-angle constraint!
     */
    inline void setEpsAngle(const double ea) {
        eps_angle_ = ea;
        sin_angle_ = fabs(sin(ea));
    }

    /** \brief Get the angle epsilon (delta) threshold. */
    inline double getEpsAngle() { return (eps_angle_); }

    /** \brief Select all the points which respect the given model coefficients
     * as inliers. \param[in] model_coefficients the coefficients of a plane
     * model that we need to compute distances to \param[in] threshold a maximum
     * admissible distance threshold for determining the inliers from the
     * outliers \param[out] inliers the resultant model inliers
     */
    void selectWithinDistance(const Eigen::VectorXf &model_coefficients,
                              const double threshold,
                              std::vector<int> &inliers);

    /** \brief Count all the points which respect the given model coefficients
     * as inliers.
     *
     * \param[in] model_coefficients the coefficients of a model that we need to
     * compute distances to \param[in] threshold maximum admissible distance
     * threshold for determining the inliers from the outliers \return the
     * resultant number of inliers
     */
    virtual int countWithinDistance(const Eigen::VectorXf &model_coefficients,
                                    const double threshold);

    /** \brief Compute all distances from the cloud data to a given plane model.
     * \param[in] model_coefficients the coefficients of a plane model that we
     * need to compute distances to \param[out] distances the resultant
     * estimated distances
     */
    void getDistancesToModel(const Eigen::VectorXf &model_coefficients,
                             std::vector<double> &distances);

    /** \brief Return an unique id for this model (SACMODEL_PARALLEL_PLANE). */
    inline pcl::SacModel getModelType() const {
        return (SACMODEL_PARALLEL_PLANE);
    }

  protected:
    /** \brief Check whether a model is valid given the user constraints.
     * \param[in] model_coefficients the set of model coefficients
     */
    bool isModelValid(const Eigen::VectorXf &model_coefficients);

    /** \brief The axis along which we need to search for a plane perpendicular
     * to. */
    Eigen::Vector3f axis_;

    /** \brief The maximum allowed difference between the plane and the given
     * axis. */
    double eps_angle_;

    /** \brief The sine of the angle*/
    double sin_angle_;
};
} // namespace pcl

#endif //#ifndef PCL_SAMPLE_CONSENSUS_MODEL_PARALLELPLANE_H_
