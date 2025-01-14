/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
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
 * $Id: extract_clusters.h 5027 2012-03-12 03:10:45Z rusu $
 *
 */

#ifndef PCL_SEGMENTATION_EDGE_AWARE_PLANE_COMPARATOR_H_
#define PCL_SEGMENTATION_EDGE_AWARE_PLANE_COMPARATOR_H_

#include <pcl/segmentation/boost.h>
#include <pcl/segmentation/plane_coefficient_comparator.h>

namespace pcl {
/** \brief EdgeAwarePlaneComparator is a Comparator that operates on plane
 * coefficients, for use in planar segmentation. In conjunction with
 * OrganizedConnectedComponentSegmentation, this allows planes to be segmented
 * from organized data.
 *
 * \author Stefan Holzer, Alex Trevor
 */
template <typename PointT, typename PointNT>
class EdgeAwarePlaneComparator
    : public PlaneCoefficientComparator<PointT, PointNT> {
  public:
    typedef typename Comparator<PointT>::PointCloud PointCloud;
    typedef typename Comparator<PointT>::PointCloudConstPtr PointCloudConstPtr;

    typedef typename pcl::PointCloud<PointNT> PointCloudN;
    typedef typename PointCloudN::Ptr PointCloudNPtr;
    typedef typename PointCloudN::ConstPtr PointCloudNConstPtr;

    typedef boost::shared_ptr<EdgeAwarePlaneComparator<PointT, PointNT>> Ptr;
    typedef boost::shared_ptr<const EdgeAwarePlaneComparator<PointT, PointNT>>
        ConstPtr;

    using pcl::PlaneCoefficientComparator<PointT, PointNT>::input_;
    using pcl::PlaneCoefficientComparator<PointT, PointNT>::normals_;
    using pcl::PlaneCoefficientComparator<PointT, PointNT>::plane_coeff_d_;
    using pcl::PlaneCoefficientComparator<PointT, PointNT>::angular_threshold_;
    using pcl::PlaneCoefficientComparator<PointT, PointNT>::distance_threshold_;

    /** \brief Empty constructor for PlaneCoefficientComparator. */
    EdgeAwarePlaneComparator() {}

    /** \brief Empty constructor for PlaneCoefficientComparator.
     * \param[in] distance_map the distance map to use
     */
    EdgeAwarePlaneComparator(const float *distance_map)
        : distance_map_(distance_map) {}

    /** \brief Destructor for PlaneCoefficientComparator. */
    virtual ~EdgeAwarePlaneComparator() {}

    /** \brief Set a distance map to use. For an example of a valid distance map
     * see \ref OrganizedIntegralImageNormalEstimation \param[in] distance_map
     * the distance map to use
     */
    inline void setDistanceMap(const float *distance_map) {
        distance_map_ = distance_map;
    }

    /** \brief Return the distance map used. */
    const float *getDistanceMap() const { return (distance_map_); }

  protected:
    /** \brief Compare two neighboring points, by using normal information,
     * curvature, and euclidean distance information. \param[in] idx1 The index
     * of the first point. \param[in] idx2 The index of the second point.
     */
    bool compare(int idx1, int idx2) const {
        float dx = input_->points[idx1].x - input_->points[idx2].x;
        float dy = input_->points[idx1].y - input_->points[idx2].y;
        float dz = input_->points[idx1].z - input_->points[idx2].z;
        float dist = sqrtf(dx * dx + dy * dy + dz * dz);

        bool normal_ok = (normals_->points[idx1].getNormalVector3fMap().dot(
                              normals_->points[idx2].getNormalVector3fMap()) >
                          angular_threshold_);
        bool dist_ok = (dist < distance_threshold_);

        bool curvature_ok = normals_->points[idx1].curvature < 0.04;
        bool plane_d_ok =
            fabs((*plane_coeff_d_)[idx1] - (*plane_coeff_d_)[idx2]) < 0.04;

        if (distance_map_[idx1] < 5) // 5 pixels
            curvature_ok = false;

        return (dist_ok && normal_ok && curvature_ok && plane_d_ok);
    }

  protected:
    const float *distance_map_;
};
} // namespace pcl

#endif // PCL_SEGMENTATION_PLANE_COEFFICIENT_COMPARATOR_H_
