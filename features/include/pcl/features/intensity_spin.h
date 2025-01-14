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
 * $Id: intensity_spin.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */
#ifndef PCL_INTENSITY_SPIN_H_
#define PCL_INTENSITY_SPIN_H_

#include <pcl/features/feature.h>

namespace pcl {
/** \brief IntensitySpinEstimation estimates the intensity-domain spin image
 * descriptors for a given point cloud dataset containing points and intensity.
 * For more information about the intensity-domain spin image descriptor, see:
 *
 *   Svetlana Lazebnik, Cordelia Schmid, and Jean Ponce.
 *   A sparse texture representation using local affine regions.
 *   In IEEE Transactions on Pattern Analysis and Machine Intelligence, volume
 * 27, pages 1265-1278, August 2005. \author Michael Dixon \ingroup features
 */
template <typename PointInT, typename PointOutT>
class IntensitySpinEstimation : public Feature<PointInT, PointOutT> {
  public:
    using Feature<PointInT, PointOutT>::feature_name_;
    using Feature<PointInT, PointOutT>::getClassName;

    using Feature<PointInT, PointOutT>::input_;
    using Feature<PointInT, PointOutT>::indices_;
    using Feature<PointInT, PointOutT>::surface_;

    using Feature<PointInT, PointOutT>::tree_;
    using Feature<PointInT, PointOutT>::search_radius_;

    typedef typename pcl::PointCloud<PointInT> PointCloudIn;
    typedef typename Feature<PointInT, PointOutT>::PointCloudOut PointCloudOut;

    /** \brief Empty constructor. */
    IntensitySpinEstimation()
        : nr_distance_bins_(4), nr_intensity_bins_(5), sigma_(1.0) {
        feature_name_ = "IntensitySpinEstimation";
    };

    /** \brief Estimate the intensity-domain spin image descriptor for a given
     * point based on its spatial neighborhood of 3D points and their
     * intensities. \param[in] cloud the dataset containing the Cartesian
     * coordinates and intensity values of the points \param[in] radius the
     * radius of the feature \param[in] sigma the standard deviation of the
     * Gaussian smoothing kernel to use during the soft histogram update
     * \param[in] k the number of neighbors to use from \a indices and \a
     * squared_distances \param[in] indices the indices of the points that
     * comprise the query point's neighborhood \param[in] squared_distances the
     * squared distances from the query point to each point in the neighborhood
     * \param[out] intensity_spin_image the resultant intensity-domain spin
     * image descriptor
     */
    void computeIntensitySpinImage(const PointCloudIn &cloud, float radius,
                                   float sigma, int k,
                                   const std::vector<int> &indices,
                                   const std::vector<float> &squared_distances,
                                   Eigen::MatrixXf &intensity_spin_image);

    /** \brief Set the number of bins to use in the distance dimension of the
     * spin image \param[in] nr_distance_bins the number of bins to use in the
     * distance dimension of the spin image
     */
    inline void setNrDistanceBins(size_t nr_distance_bins) {
        nr_distance_bins_ = static_cast<int>(nr_distance_bins);
    };

    /** \brief Returns the number of bins in the distance dimension of the spin
     * image. */
    inline int getNrDistanceBins() { return (nr_distance_bins_); };

    /** \brief Set the number of bins to use in the intensity dimension of the
     * spin image. \param[in] nr_intensity_bins the number of bins to use in the
     * intensity dimension of the spin image
     */
    inline void setNrIntensityBins(size_t nr_intensity_bins) {
        nr_intensity_bins_ = static_cast<int>(nr_intensity_bins);
    };

    /** \brief Returns the number of bins in the intensity dimension of the spin
     * image. */
    inline int getNrIntensityBins() { return (nr_intensity_bins_); };

    /** \brief Set the standard deviation of the Gaussian smoothing kernel to
     * use when constructing the spin images. \param[in] sigma the standard
     * deviation of the Gaussian smoothing kernel to use when constructing the
     * spin images
     */
    inline void setSmoothingBandwith(float sigma) { sigma_ = sigma; };

    /** \brief Returns the standard deviation of the Gaussian smoothing kernel
     * used to construct the spin images.  */
    inline float getSmoothingBandwith() { return (sigma_); };

    /** \brief Estimate the intensity-domain descriptors at a set of points
     * given by <setInputCloud (), setIndices ()> using the surface in
     * setSearchSurface (), and the spatial locator in setSearchMethod ().
     * \param[out] output the resultant point cloud model dataset that contains
     * the intensity-domain spin image features
     */
    void computeFeature(PointCloudOut &output);

    /** \brief The number of distance bins in the descriptor. */
    int nr_distance_bins_;

    /** \brief The number of intensity bins in the descriptor. */
    int nr_intensity_bins_;

    /** \brief The standard deviation of the Gaussian smoothing kernel used to
     * construct the spin images. */
    float sigma_;

  private:
    /** \brief Make the computeFeature (&Eigen::MatrixXf); inaccessible from
     * outside the class \param[out] output the output point cloud
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &) {}
};

/** \brief IntensitySpinEstimation estimates the intensity-domain spin image
 * descriptors for a given point cloud dataset containing points and intensity.
 * For more information about the intensity-domain spin image descriptor, see:
 *
 *   Svetlana Lazebnik, Cordelia Schmid, and Jean Ponce.
 *   A sparse texture representation using local affine regions.
 *   In IEEE Transactions on Pattern Analysis and Machine Intelligence, volume
 * 27, pages 1265-1278, August 2005. \author Michael Dixon \ingroup features
 */
template <typename PointInT>
class IntensitySpinEstimation<PointInT, Eigen::MatrixXf>
    : public IntensitySpinEstimation<PointInT, pcl::Histogram<20>> {
  public:
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::getClassName;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::input_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::indices_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::surface_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::search_radius_;
    using IntensitySpinEstimation<PointInT,
                                  pcl::Histogram<20>>::nr_intensity_bins_;
    using IntensitySpinEstimation<PointInT,
                                  pcl::Histogram<20>>::nr_distance_bins_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::tree_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::sigma_;
    using IntensitySpinEstimation<PointInT, pcl::Histogram<20>>::compute;

  private:
    /** \brief Estimate the intensity-domain descriptors at a set of points
     * given by <setInputCloud (), setIndices ()> using the surface in
     * setSearchSurface (), and the spatial locator in setSearchMethod ().
     * \param[out] output the resultant point cloud model dataset that contains
     * the intensity-domain spin image features
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &output);

    /** \brief Make the compute (&PointCloudOut); inaccessible from outside the
     * class \param[out] output the output point cloud
     */
    void compute(pcl::PointCloud<pcl::Normal> &) {}
};
} // namespace pcl

#endif // #ifndef PCL_INTENSITY_SPIN_H_
