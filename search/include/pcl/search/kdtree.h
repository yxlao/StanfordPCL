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
 * $Id: kdtree.h 6459 2012-07-18 07:50:37Z dpb $
 */

#ifndef PCL_SEARCH_KDTREE_H_
#define PCL_SEARCH_KDTREE_H_

#include <pcl/search/search.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/kdtree/kdtree_flann.h>

namespace pcl {
namespace search {
/** \brief @b search::KdTree is a wrapper class which inherits the pcl::KdTree
 * class for performing search functions using KdTree structure. KdTree is a
 * generic type of 3D spatial locator using kD-tree structures. The class is
 * making use of the FLANN (Fast Library for Approximate Nearest Neighbor)
 * project by Marius Muja and David Lowe.
 *
 * \author Radu B. Rusu
 * \ingroup search
 */
template <typename PointT> class KdTree : public Search<PointT> {
  public:
    typedef typename Search<PointT>::PointCloud PointCloud;
    typedef typename Search<PointT>::PointCloudConstPtr PointCloudConstPtr;

    typedef boost::shared_ptr<std::vector<int>> IndicesPtr;
    typedef boost::shared_ptr<const std::vector<int>> IndicesConstPtr;

    using pcl::search::Search<PointT>::indices_;
    using pcl::search::Search<PointT>::input_;
    using pcl::search::Search<PointT>::getIndices;
    using pcl::search::Search<PointT>::getInputCloud;
    using pcl::search::Search<PointT>::nearestKSearch;
    using pcl::search::Search<PointT>::radiusSearch;
    using pcl::search::Search<PointT>::sorted_results_;

    typedef boost::shared_ptr<KdTree<PointT>> Ptr;
    typedef boost::shared_ptr<const KdTree<PointT>> ConstPtr;

    typedef boost::shared_ptr<pcl::KdTreeFLANN<PointT>> KdTreeFLANNPtr;
    typedef boost::shared_ptr<const pcl::KdTreeFLANN<PointT>>
        KdTreeFLANNConstPtr;

    /** \brief Constructor for KdTree.
     *
     * \param sorted set to true if the nearest neighbor search results
     * need to be sorted in ascending order based on their distance to the
     * query point
     *
     */
    KdTree(bool sorted = true)
        : Search<PointT>("KdTree", sorted),
          tree_(new pcl::KdTreeFLANN<PointT>(sorted)) {}

    /** \brief Destructor for KdTree. */
    virtual ~KdTree() {}

    /** \brief Sets whether the results have to be sorted or not.
     * \param[in] sorted_results set to true if the radius search results should
     * be sorted
     */
    virtual void setSortedResults(bool sorted_results) {
        sorted_results_ = sorted_results;
        tree_->setSortedResults(sorted_results);
    }

    /** \brief Set the search epsilon precision (error bound) for nearest
     * neighbors searches. \param[in] eps precision (error bound) for nearest
     * neighbors searches
     */
    inline void setEpsilon(float eps) { tree_->setEpsilon(eps); }

    /** \brief Get the search epsilon precision (error bound) for nearest
     * neighbors searches. */
    inline float getEpsilon() const { return (tree_->getEpsilon()); }

    /** \brief Provide a pointer to the input dataset.
     * \param[in] cloud the const boost shared pointer to a PointCloud message
     * \param[in] indices the point indices subset that is to be used from \a
     * cloud
     */
    inline void
    setInputCloud(const PointCloudConstPtr &cloud,
                  const IndicesConstPtr &indices = IndicesConstPtr()) {
        tree_->setInputCloud(cloud, indices);
        input_ = cloud;
        indices_ = indices;
    }

    /** \brief Search for the k-nearest neighbors for the given query point.
     * \param[in] point the given query point
     * \param[in] k the number of neighbors to search for
     * \param[out] k_indices the resultant indices of the neighboring points
     * (must be resized to \a k a priori!) \param[out] k_sqr_distances the
     * resultant squared distances to the neighboring points (must be resized to
     * \a k a priori!) \return number of neighbors found
     */
    inline int nearestKSearch(const PointT &point, int k,
                              std::vector<int> &k_indices,
                              std::vector<float> &k_sqr_distances) const {
        return (tree_->nearestKSearch(point, k, k_indices, k_sqr_distances));
    }

    /** \brief Search for all the nearest neighbors of the query point in a
     * given radius. \param[in] point the given query point \param[in] radius
     * the radius of the sphere bounding all of p_q's neighbors \param[out]
     * k_indices the resultant indices of the neighboring points \param[out]
     * k_sqr_distances the resultant squared distances to the neighboring points
     * \param[in] max_nn if given, bounds the maximum returned neighbors to this
     * value. If \a max_nn is set to 0 or to a number higher than the number of
     * points in the input cloud, all neighbors in \a radius will be returned.
     * \return number of neighbors found in radius
     */
    inline int radiusSearch(const PointT &point, double radius,
                            std::vector<int> &k_indices,
                            std::vector<float> &k_sqr_distances,
                            unsigned int max_nn = 0) const {
        return (tree_->radiusSearch(point, radius, k_indices, k_sqr_distances,
                                    max_nn));
    }

  protected:
    /** \brief A pointer to the internal KdTreeFLANN object. */
    KdTreeFLANNPtr tree_;
};
} // namespace search
} // namespace pcl

#define PCL_INSTANTIATE_KdTree(T)                                              \
    template class PCL_EXPORTS pcl::search::KdTree<T>;

#endif // PCL_SEARCH_KDTREE_H_
