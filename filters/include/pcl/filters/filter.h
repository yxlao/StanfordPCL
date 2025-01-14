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
 * $Id: filter.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_FILTER_H_
#define PCL_FILTER_H_

#include <pcl/pcl_base.h>
#include <pcl/ros/conversions.h>
#include <pcl/filters/boost.h>
#include <cfloat>
#include <pcl/PointIndices.h>

namespace pcl {
/** \brief Removes points with x, y, or z equal to NaN
 * \param[in] cloud_in the input point cloud
 * \param[out] cloud_out the input point cloud
 * \param[out] index the mapping (ordered): cloud_out.points[i] =
 * cloud_in.points[index[i]] \note The density of the point cloud is lost. \note
 * Can be called with cloud_in == cloud_out \ingroup filters
 */
template <typename PointT>
void removeNaNFromPointCloud(const pcl::PointCloud<PointT> &cloud_in,
                             pcl::PointCloud<PointT> &cloud_out,
                             std::vector<int> &index);

////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Filter represents the base filter class. All filters must inherit
 * from this interface. \author Radu B. Rusu \ingroup filters
 */
template <typename PointT> class Filter : public PCLBase<PointT> {
  public:
    using PCLBase<PointT>::indices_;
    using PCLBase<PointT>::input_;

    typedef boost::shared_ptr<Filter<PointT>> Ptr;
    typedef boost::shared_ptr<const Filter<PointT>> ConstPtr;

    typedef pcl::PointCloud<PointT> PointCloud;
    typedef typename PointCloud::Ptr PointCloudPtr;
    typedef typename PointCloud::ConstPtr PointCloudConstPtr;

    /** \brief Empty constructor.
     * \param[in] extract_removed_indices set to true if the filtered data
     * indices should be saved in a separate list. Default: false.
     */
    Filter(bool extract_removed_indices = false)
        : removed_indices_(new std::vector<int>), filter_name_(),
          extract_removed_indices_(extract_removed_indices) {}

    /** \brief Get the point indices being removed */
    inline IndicesConstPtr const getRemovedIndices() {
        return (removed_indices_);
    }

    /** \brief Get the point indices being removed
     * \param[out] pi the resultant point indices that have been removed
     */
    inline void getRemovedIndices(PointIndices &pi) {
        pi.indices = *removed_indices_;
    }

    /** \brief Calls the filtering method and returns the filtered dataset in
     * output. \param[out] output the resultant filtered point cloud dataset
     */
    inline void filter(PointCloud &output) {
        if (!initCompute())
            return;

        // Resize the output dataset
        // if (output.points.size () != indices_->size ())
        //  output.points.resize (indices_->size ());

        // Copy header at a minimum
        output.header = input_->header;
        output.sensor_origin_ = input_->sensor_origin_;
        output.sensor_orientation_ = input_->sensor_orientation_;

        // Apply the actual filter
        applyFilter(output);

        deinitCompute();
    }

  protected:
    using PCLBase<PointT>::initCompute;
    using PCLBase<PointT>::deinitCompute;

    /** \brief Indices of the points that are removed */
    IndicesPtr removed_indices_;

    /** \brief The filter name. */
    std::string filter_name_;

    /** \brief Set to true if we want to return the indices of the removed
     * points. */
    bool extract_removed_indices_;

    /** \brief Abstract filter method.
     *
     * The implementation needs to set output.{points, width, height, is_dense}.
     *
     * \param[out] output the resultant filtered point cloud
     */
    virtual void applyFilter(PointCloud &output) = 0;

    /** \brief Get a string representation of the name of this class. */
    inline const std::string &getClassName() const { return (filter_name_); }
};

////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Filter represents the base filter class. All filters must inherit
 * from this interface. \author Radu B. Rusu \ingroup filters
 */
template <>
class PCL_EXPORTS Filter<sensor_msgs::PointCloud2>
    : public PCLBase<sensor_msgs::PointCloud2> {
  public:
    typedef sensor_msgs::PointCloud2 PointCloud2;
    typedef PointCloud2::Ptr PointCloud2Ptr;
    typedef PointCloud2::ConstPtr PointCloud2ConstPtr;

    /** \brief Empty constructor.
     * \param[in] extract_removed_indices set to true if the filtered data
     * indices should be saved in a separate list. Default: false.
     */
    Filter(bool extract_removed_indices = false)
        : removed_indices_(new std::vector<int>),
          extract_removed_indices_(extract_removed_indices), filter_name_() {}

    /** \brief Get the point indices being removed */
    inline IndicesConstPtr const getRemovedIndices() {
        return (removed_indices_);
    }

    /** \brief Get the point indices being removed
     * \param[out] pi the resultant point indices that have been removed
     */
    inline void getRemovedIndices(PointIndices &pi) {
        pi.indices = *removed_indices_;
    }

    /** \brief Calls the filtering method and returns the filtered dataset in
     * output. \param[out] output the resultant filtered point cloud dataset
     */
    void filter(PointCloud2 &output);

  protected:
    /** \brief Indices of the points that are removed */
    IndicesPtr removed_indices_;

    /** \brief Set to true if we want to return the indices of the removed
     * points. */
    bool extract_removed_indices_;

    /** \brief The filter name. */
    std::string filter_name_;

    /** \brief Abstract filter method.
     *
     * The implementation needs to set output.{data, row_step, point_step,
     * width, height, is_dense}.
     *
     * \param[out] output the resultant filtered point cloud
     */
    virtual void applyFilter(PointCloud2 &output) = 0;

    /** \brief Get a string representation of the name of this class. */
    inline const std::string &getClassName() const { return (filter_name_); }
};
} // namespace pcl

#endif //#ifndef PCL_FILTER_H_
