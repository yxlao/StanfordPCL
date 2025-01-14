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
 *  $Id: usc.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_FEATURES_USC_H_
#define PCL_FEATURES_USC_H_

#include <pcl/point_types.h>
#include <pcl/features/feature.h>

namespace pcl {
/** \brief UniqueShapeContext implements the Unique Shape Context Descriptor
 * described here:
 *
 *   - F. Tombari, S. Salti, L. Di Stefano,
 *     "Unique Shape Context for 3D data description",
 *     International Workshop on 3D Object Retrieval (3DOR 10) -
 *     in conjuction with ACM Multimedia 2010
 *
 * The suggested PointOutT is pcl::ShapeContext1980
 *
 * \author Alessandro Franchi, Federico Tombari, Samuele Salti (original code)
 * \author Nizar Sallem (port to PCL)
 * \ingroup features
 */
template <typename PointInT, typename PointOutT = pcl::ShapeContext1980,
          typename PointRFT = pcl::ReferenceFrame>
class UniqueShapeContext
    : public Feature<PointInT, PointOutT>,
      public FeatureWithLocalReferenceFrames<PointInT, PointRFT> {
  public:
    using Feature<PointInT, PointOutT>::feature_name_;
    using Feature<PointInT, PointOutT>::getClassName;
    using Feature<PointInT, PointOutT>::indices_;
    using Feature<PointInT, PointOutT>::search_parameter_;
    using Feature<PointInT, PointOutT>::search_radius_;
    using Feature<PointInT, PointOutT>::surface_;
    using Feature<PointInT, PointOutT>::fake_surface_;
    using Feature<PointInT, PointOutT>::input_;
    using Feature<PointInT, PointOutT>::searchForNeighbors;
    using FeatureWithLocalReferenceFrames<PointInT, PointRFT>::frames_;

    typedef typename Feature<PointInT, PointOutT>::PointCloudOut PointCloudOut;
    typedef typename Feature<PointInT, PointOutT>::PointCloudIn PointCloudIn;
    typedef typename boost::shared_ptr<
        UniqueShapeContext<PointInT, PointOutT, PointRFT>>
        Ptr;
    typedef typename boost::shared_ptr<
        const UniqueShapeContext<PointInT, PointOutT, PointRFT>>
        ConstPtr;

    /** \brief Constructor. */
    UniqueShapeContext()
        : radii_interval_(0), theta_divisions_(0), phi_divisions_(0),
          volume_lut_(0), azimuth_bins_(12), elevation_bins_(11),
          radius_bins_(15), min_radius_(0.1), point_density_radius_(0.2),
          descriptor_length_(), local_radius_(2.5) {
        feature_name_ = "UniqueShapeContext";
        search_radius_ = 2.5;
    }

    virtual ~UniqueShapeContext() {}

    // inline void
    // setAzimuthBins (size_t bins) { azimuth_bins_ = bins; }

    /** \return The number of bins along the azimuth. */
    inline size_t getAzimuthBins() const { return (azimuth_bins_); }

    // inline void
    // setElevationBins (size_t bins) { elevation_bins_ = bins; }

    /** \return The number of bins along the elevation */
    inline size_t getElevationBins() const { return (elevation_bins_); }

    // inline void
    // setRadiusBins (size_t bins) { radius_bins_ = bins; }

    /** \return The number of bins along the radii direction. */
    inline size_t getRadiusBins() const { return (radius_bins_); }

    /** The minimal radius value for the search sphere (rmin) in the original
     * paper \param[in] radius the desired minimal radius
     */
    inline void setMinimalRadius(double radius) { min_radius_ = radius; }

    /** \return The minimal sphere radius. */
    inline double getMinimalRadius() const { return (min_radius_); }

    /** This radius is used to compute local point density
     * density = number of points within this radius
     * \param[in] radius Value of the point density search radius
     */
    inline void setPointDensityRadius(double radius) {
        point_density_radius_ = radius;
    }

    /** \return The point density search radius. */
    inline double getPointDensityRadius() const {
        return (point_density_radius_);
    }

    /** Set the local RF radius value
     * \param[in] radius the desired local RF radius
     */
    inline void setLocalRadius(double radius) { local_radius_ = radius; }

    /** \return The local RF radius. */
    inline double getLocalRadius() const { return (local_radius_); }

  protected:
    /** Compute 3D shape context feature descriptor
     * \param[in] index point index in input_
     * \param[out] desc descriptor to compute
     */
    void computePointDescriptor(size_t index, std::vector<float> &desc);

    /** \brief Initialize computation by allocating all the intervals and the
     * volume lookup table. */
    virtual bool initCompute();

    /** \brief The actual feature computation.
     * \param[out] output the resultant features
     */
    virtual void computeFeature(PointCloudOut &output);

    /** \brief values of the radii interval. */
    std::vector<float> radii_interval_;

    /** \brief Theta divisions interval. */
    std::vector<float> theta_divisions_;

    /** \brief Phi divisions interval. */
    std::vector<float> phi_divisions_;

    /** \brief Volumes look up table. */
    std::vector<float> volume_lut_;

    /** \brief Bins along the azimuth dimension. */
    size_t azimuth_bins_;

    /** \brief Bins along the elevation dimension. */
    size_t elevation_bins_;

    /** \brief Bins along the radius dimension. */
    size_t radius_bins_;

    /** \brief Minimal radius value. */
    double min_radius_;

    /** \brief Point density radius. */
    double point_density_radius_;

    /** \brief Descriptor length. */
    size_t descriptor_length_;

    /** \brief Radius to compute local RF. */
    double local_radius_;

  private:
    /** \brief Make the computeFeature (&Eigen::MatrixXf); inaccessible from
     * outside the class \param[out] output the output point cloud
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &) {}
};

/** \brief UniqueShapeContext implements the Unique Shape Descriptor
 * described here:
 *
 *   - F. Tombari, S. Salti, L. Di Stefano,
 *     "Unique Shape Context for 3D data description",
 *     International Workshop on 3D Object Retrieval (3DOR 10) -
 *     in conjuction with ACM Multimedia 2010
 *
 * \author Alessandro Franchi, Federico Tombari, Samuele Salti (original code)
 * \author Nizar Sallem (port to PCL)
 * \ingroup features
 */
template <typename PointInT, typename PointRFT>
class UniqueShapeContext<PointInT, Eigen::MatrixXf, PointRFT>
    : public UniqueShapeContext<PointInT, pcl::ShapeContext1980, PointRFT> {
  public:
    using FeatureWithLocalReferenceFrames<PointInT, PointRFT>::frames_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980, PointRFT>::input_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::indices_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::descriptor_length_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::compute;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::computePointDescriptor;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::azimuth_bins_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::elevation_bins_;
    using UniqueShapeContext<PointInT, pcl::ShapeContext1980,
                             PointRFT>::radius_bins_;

    /** \brief Set the number of bins along the azimuth
     * \param[in] bins the number of bins along the azimuth
     */
    inline void setAzimuthBins(size_t bins) { azimuth_bins_ = bins; }

    /** \brief Set the number of bins along the elevation
     * \param[in] bins the number of bins along the elevation
     */
    inline void setElevationBins(size_t bins) { elevation_bins_ = bins; }

    /** \brief Set the number of bins along the radii
     * \param[in] bins the number of bins along the radii
     */
    inline void setRadiusBins(size_t bins) { radius_bins_ = bins; }

  private:
    /** \brief The actual feature computation.
     * \param[out] output the resultant features
     */
    virtual void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &output);

    /** \brief Make the compute (&PointCloudOut); inaccessible from outside the
     * class \param[out] output the output point cloud
     */
    void compute(pcl::PointCloud<pcl::ShapeContext1980> &) {}
};
} // namespace pcl

#endif //#ifndef PCL_USC_H_
