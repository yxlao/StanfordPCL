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
 * $Id: correspondence_rejection_distance.h 7153 2012-09-16 22:24:29Z aichim $
 *
 */
#ifndef PCL_REGISTRATION_CORRESPONDENCE_REJECTION_DISTANCE_H_
#define PCL_REGISTRATION_CORRESPONDENCE_REJECTION_DISTANCE_H_

#include <pcl/registration/correspondence_rejection.h>

namespace pcl {
namespace registration {
/**
 * @b CorrespondenceRejectorDistance implements a simple correspondence
 * rejection method based on thresholding the distances between the
 * correspondences.
 *
 * \note If \ref setInputCloud and \ref setInputTarget are given, then the
 * distances between correspondences will be estimated using the given XYZ
 * data, and not read from the set of input correspondences.
 *
 * \author Dirk Holz, Radu B. Rusu
 * \ingroup registration
 */
class CorrespondenceRejectorDistance : public CorrespondenceRejector {
    using CorrespondenceRejector::getClassName;
    using CorrespondenceRejector::input_correspondences_;
    using CorrespondenceRejector::rejection_name_;

  public:
    /** \brief Empty constructor. */
    CorrespondenceRejectorDistance()
        : max_distance_(std::numeric_limits<float>::max()), data_container_() {
        rejection_name_ = "CorrespondenceRejectorDistance";
    }

    /** \brief Get a list of valid correspondences after rejection from the
     * original set of correspondences. \param[in] original_correspondences the
     * set of initial correspondences given \param[out]
     * remaining_correspondences the resultant filtered set of remaining
     * correspondences
     */
    inline void getRemainingCorrespondences(
        const pcl::Correspondences &original_correspondences,
        pcl::Correspondences &remaining_correspondences);

    /** \brief Set the maximum distance used for thresholding in correspondence
     * rejection. \param[in] distance Distance to be used as maximum distance
     * between correspondences. Correspondences with larger distances are
     * rejected. \note Internally, the distance will be stored squared.
     */
    virtual inline void setMaximumDistance(float distance) {
        max_distance_ = distance * distance;
    };

    /** \brief Get the maximum distance used for thresholding in correspondence
     * rejection. */
    inline float getMaximumDistance() { return std::sqrt(max_distance_); };

    /** \brief Provide a source point cloud dataset (must contain XYZ
     * data!), used to compute the correspondence distance.
     * \param[in] cloud a cloud containing XYZ data
     */
    template <typename PointT>
    inline void
    setInputCloud(const typename pcl::PointCloud<PointT>::ConstPtr &cloud) {
        if (!data_container_)
            data_container_.reset(new DataContainer<PointT>);
        boost::static_pointer_cast<DataContainer<PointT>>(data_container_)
            ->setInputCloud(cloud);
    }

    /** \brief Provide a target point cloud dataset (must contain XYZ
     * data!), used to compute the correspondence distance.
     * \param[in] target a cloud containing XYZ data
     */
    template <typename PointT>
    inline void
    setInputTarget(const typename pcl::PointCloud<PointT>::ConstPtr &target) {
        if (!data_container_)
            data_container_.reset(new DataContainer<PointT>);
        boost::static_pointer_cast<DataContainer<PointT>>(data_container_)
            ->setInputTarget(target);
    }

  protected:
    /** \brief Apply the rejection algorithm.
     * \param[out] correspondences the set of resultant correspondences.
     */
    inline void applyRejection(pcl::Correspondences &correspondences) {
        getRemainingCorrespondences(*input_correspondences_, correspondences);
    }

    /** \brief The maximum distance threshold between two correspondent points
     * in source <-> target. If the distance is larger than this threshold, the
     * points will not be ignored in the alignment process.
     */
    float max_distance_;

    typedef boost::shared_ptr<DataContainerInterface> DataContainerPtr;

    /** \brief A pointer to the DataContainer object containing the input and
     * target point clouds */
    DataContainerPtr data_container_;
};

} // namespace registration
} // namespace pcl

#include <pcl/registration/impl/correspondence_rejection_distance.hpp>

#endif /* PCL_REGISTRATION_CORRESPONDENCE_REJECTION_DISTANCE_H_ */
