/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2012-, Open Perception , Inc.
 *  Copyright (C) 2011  The Autonomous Systems Lab (ASL), ETH Zurich,
 *                      Stefan Leutenegger, Simon Lynen and Margarita Chli.
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
 */

#ifndef PCL_FEATURES_BRISK_2D_H_
#define PCL_FEATURES_BRISK_2D_H_

// PCL includes
#include <pcl/features/feature.h>
#include <pcl/common/eigen.h>
#include <pcl/common/centroid.h>
#include <pcl/common/intensity.h>

namespace pcl {
/** \brief Implementation of the BRISK-descriptor, based on the original code
 * and paper reference by
 *
 * \par
 * Stefan Leutenegger,Margarita Chli and Roland Siegwart,
 * BRISK: Binary Robust Invariant Scalable Keypoints,
 * in Proceedings of the IEEE International Conference on Computer Vision
 * (ICCV2011).
 *
 * \author Radu B. Rusu, Stefan Holzer
 * \ingroup features
 */
template <typename PointInT, typename PointOutT = pcl::BRISKSignature512,
          typename KeypointT = pcl::PointWithScale,
          typename IntensityT = pcl::common::IntensityFieldAccessor<PointInT>>
class BRISK2DEstimation // : public Feature<PointT, KeyPointT>
{
  public:
    typedef typename pcl::PointCloud<PointInT> PointCloudInT;
    typedef typename pcl::PointCloud<PointInT>::ConstPtr PointCloudInTConstPtr;

    typedef typename pcl::PointCloud<KeypointT> KeypointPointCloudT;
    typedef typename pcl::PointCloud<KeypointT>::Ptr KeypointPointCloudTPtr;
    typedef typename pcl::PointCloud<KeypointT>::ConstPtr
        KeypointPointCloudTConstPtr;

    typedef typename pcl::PointCloud<PointOutT> PointCloudOutT;

    /** \brief Constructor. */
    BRISK2DEstimation();

    /** \brief Destructor. */
    virtual ~BRISK2DEstimation();

    /** \brief Determines whether rotation invariance is enabled.
     * \param[in] enable determines whether rotation invariance is enabled.
     */
    inline void setRotationInvariance(const bool enable) {
        rotation_invariance_enabled_ = enable;
    }

    /** \brief Determines whether scale invariance is enabled.
     * \param[in] enable determines whether scale invariance is enabled.
     */
    inline void setScaleInvariance(const bool enable) {
        scale_invariance_enabled_ = enable;
    }

    /** \brief Sets the input cloud.
     * \param[in] cloud the input cloud.
     */
    inline void setInputCloud(const PointCloudInTConstPtr &cloud) {
        input_cloud_ = cloud;
    }

    /** \brief Sets the input keypoints.
     * \param[in] keypoints the input cloud containing the keypoints.
     */
    inline void setKeypoints(const KeypointPointCloudTConstPtr &keypoints) {
        // Make a copy as we will have to erase keypoints that we don't use
        // TO DO: change this later
        keypoints_.reset(new KeypointPointCloudT(*keypoints));
    }

    /** \brief Computes the descriptors for the previously specified
     *        points and input data.
     * \param[out] descriptors the destination for the computed descriptors.
     */
    void compute(PointCloudOutT &output);
    // td::vector<pcl::features::brisk::BRISKDescriptor> & descriptors) const;

  protected:
    /** \brief Call this to generate the kernel:
     * circle of radius r (pixels), with n points;
     * short pairings with dMax, long pairings with dMin
     *
     * \note This should never be called by a regular user. We use a fixed type
     * in PCL (BRISKSignature512) and tampering with the parameters might lead
     * to a different size descriptor which the user needs to accomodate in a
     * new point type.
     */
    void generateKernel(std::vector<float> &radius_list,
                        std::vector<int> &number_list, float d_max = 5.85f,
                        float d_min = 8.2f,
                        std::vector<int> index_change = std::vector<int>());

    /** \brief Compute the smoothed intensity for a given x/y position in the
     * image. */
    inline int smoothedIntensity(const std::vector<unsigned char> &image,
                                 int image_width, int image_height,
                                 const std::vector<int> &integral_image,
                                 const float key_x, const float key_y,
                                 const unsigned int scale,
                                 const unsigned int rot,
                                 const unsigned int point) const;

  private:
    /** \brief ROI predicate comparator. */
    bool RoiPredicate(const float min_x, const float min_y, const float max_x,
                      const float max_y, const KeypointT &key_pt);

    /** \brief Specifies whether rotation invariance is enabled. */
    bool rotation_invariance_enabled_;

    /** \brief Specifies whether scale invariance is enabled. */
    bool scale_invariance_enabled_;

    /** \brief Specifies the scale of the pattern. */
    const float pattern_scale_;

    /** \brief the input cloud. */
    PointCloudInTConstPtr input_cloud_;

    /** \brief the input keypoints. */
    KeypointPointCloudTPtr keypoints_;

    // TODO: set
    float scale_range_;

    // Some helper structures for the Brisk pattern representation
    struct BriskPatternPoint {
        /** x coordinate relative to center. */
        float x;
        /** x coordinate relative to center. */
        float y;
        /** Gaussian smoothing sigma. */
        float sigma;
    };

    struct BriskShortPair {
        /** index of the first pattern point. */
        unsigned int i;
        /** index of other pattern point. */
        unsigned int j;
    };

    struct BriskLongPair {
        /** index of the first pattern point. */
        unsigned int i;
        /** index of other pattern point. */
        unsigned int j;
        /** 1024.0/dx. */
        int weighted_dx;
        /** 1024.0/dy. */
        int weighted_dy;
    };

    // pattern properties
    /** [i][rotation][scale]. */
    BriskPatternPoint *pattern_points_;

    /** Total number of collocation points. */
    unsigned int points_;

    /** Discretization of the rotation look-up. */
    const unsigned int n_rot_;

    /** Lists the scaling per scale index [scale]. */
    float *scale_list_;

    /** Lists the total pattern size per scale index [scale]. */
    unsigned int *size_list_;

    /** Scales discretization. */
    const unsigned int scales_;

    /** Span of sizes 40->4 Octaves - else, this needs to be adjusted... */
    const float scalerange_;

    // general
    const float basic_size_;

    // pairs
    /** Number of uchars the descriptor consists of. */
    int strings_;
    /** Short pair maximum distance. */
    float d_max_;
    /** Long pair maximum distance. */
    float d_min_;
    /** d<_d_max. */
    BriskShortPair *short_pairs_;
    /** d>_d_min. */
    BriskLongPair *long_pairs_;
    /** Number of short pairs. */
    unsigned int no_short_pairs_;
    /** Number of long pairs. */
    unsigned int no_long_pairs_;

    /** \brief Intensity field accessor. */
    IntensityT intensity_;

    /** \brief The name of the class. */
    std::string name_;
};

} // namespace pcl

#include <pcl/features/impl/brisk_2d.hpp>

#endif //#ifndef PCL_FEATURES_BRISK_2D_H_
