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
 * $Id: flann_search.h 7185 2012-09-18 09:21:26Z amuetzel $
 */

#ifndef PCL_SEARCH_FLANN_SEARCH_H_
#define PCL_SEARCH_FLANN_SEARCH_H_

#include <pcl/search/search.h>
#include <pcl/common/time.h>
#include <pcl/point_representation.h>

namespace flann {
template <typename T> class NNIndex;
template <typename T> struct L2;
template <typename T> struct L2_Simple;
template <typename T> class Matrix;
} // namespace flann

namespace pcl {
namespace search {

/** \brief @b search::FlannSearch is a generic FLANN wrapper class for the new
 * search interface. It is able to wrap any FLANN index type, e.g. the kd tree
 * as well as indices for high-dimensional searches and intended as a more
 * powerful and cleaner successor to KdTreeFlann.
 *
 * \author Andreas Muetzel
 * \ingroup search
 */
template <typename PointT, typename FlannDistance = flann::L2_Simple<float>>
class FlannSearch : public Search<PointT> {
    typedef typename Search<PointT>::PointCloud PointCloud;
    typedef typename Search<PointT>::PointCloudConstPtr PointCloudConstPtr;

    typedef boost::shared_ptr<std::vector<int>> IndicesPtr;
    typedef boost::shared_ptr<const std::vector<int>> IndicesConstPtr;
    typedef flann::NNIndex<FlannDistance> Index;
    typedef boost::shared_ptr<flann::NNIndex<FlannDistance>> IndexPtr;
    typedef boost::shared_ptr<flann::Matrix<float>> MatrixPtr;
    typedef boost::shared_ptr<const flann::Matrix<float>> MatrixConstPtr;

    typedef pcl::PointRepresentation<PointT> PointRepresentation;
    // typedef boost::shared_ptr<PointRepresentation> PointRepresentationPtr;
    typedef boost::shared_ptr<const PointRepresentation>
        PointRepresentationConstPtr;

    using Search<PointT>::input_;
    using Search<PointT>::indices_;
    using Search<PointT>::sorted_results_;

  public:
    typedef boost::shared_ptr<FlannSearch<PointT, FlannDistance>> Ptr;
    typedef boost::shared_ptr<const FlannSearch<PointT, FlannDistance>>
        ConstPtr;

    /** \brief Helper class that creates a FLANN index from a given FLANN
     * matrix. To use a FLANN index type with FlannSearch, implement this
     * interface and pass an object of the new type to the FlannSearch
     * constructor. See the implementation of KdTreeIndexCreator for an example.
     */
    class FlannIndexCreator {
      public:
        /** \brief Create a FLANN Index from the input data.
         * \param[in] data The FLANN matrix containing the input.
         * \return The FLANN index.
         */
        virtual IndexPtr createIndex(MatrixConstPtr data) = 0;

        /** \brief destructor
         */
        virtual ~FlannIndexCreator() {}
    };
    typedef boost::shared_ptr<FlannIndexCreator> FlannIndexCreatorPtr;

    /** \brief Creates a FLANN KdTreeSingleIndex from the given input data.
     */
    class KdTreeIndexCreator : public FlannIndexCreator {
      public:
        /** \param[in] max_leaf_size All FLANN kd trees created by this class
         * will have a maximum of max_leaf_size points per leaf node. Higher
         * values make index creation cheaper, but search more costly (and the
         * other way around).
         */
        KdTreeIndexCreator(unsigned int max_leaf_size = 15)
            : max_leaf_size_(max_leaf_size) {}
        /** \brief Create a FLANN Index from the input data.
         * \param[in] data The FLANN matrix containing the input.
         * \return The FLANN index.
         */
        virtual IndexPtr createIndex(MatrixConstPtr data);

      private:
        unsigned int max_leaf_size_;
    };

    FlannSearch(bool sorted = true,
                FlannIndexCreatorPtr creator =
                    FlannIndexCreatorPtr(new KdTreeIndexCreator()));

    /** \brief Destructor for FlannSearch. */
    virtual ~FlannSearch();

    // void
    // setInputCloud (const PointCloudConstPtr &cloud, const IndicesConstPtr
    // &indices = IndicesConstPtr ());

    /** \brief Set the search epsilon precision (error bound) for nearest
     * neighbors searches. \param[in] eps precision (error bound) for nearest
     * neighbors searches
     */
    inline void setEpsilon(double eps) { eps_ = eps; }

    /** \brief Get the search epsilon precision (error bound) for nearest
     * neighbors searches. */
    inline double getEpsilon() { return (eps_); }

    /** \brief Provide a pointer to the input dataset.
     * \param[in] cloud the const boost shared pointer to a PointCloud message
     * \param[in] indices the point indices subset that is to be used from \a
     * cloud
     */
    virtual void
    setInputCloud(const PointCloudConstPtr &cloud,
                  const IndicesConstPtr &indices = IndicesConstPtr());

    /** \brief Search for the k-nearest neighbors for the given query point.
     * \param[in] point the given query point
     * \param[in] k the number of neighbors to search for
     * \param[out] k_indices the resultant indices of the neighboring points
     * (must be resized to \a k a priori!) \param[out] k_sqr_distances the
     * resultant squared distances to the neighboring points (must be resized to
     * \a k a priori!) \return number of neighbors found
     */
    int nearestKSearch(const PointT &point, int k, std::vector<int> &k_indices,
                       std::vector<float> &k_sqr_distances) const;

    /** \brief Search for the k-nearest neighbors for the given query point.
     * \param[in] cloud the point cloud data
     * \param[in] indices a vector of point cloud indices to query for nearest
     * neighbors \param[in] k the number of neighbors to search for \param[out]
     * k_indices the resultant indices of the neighboring points, k_indices[i]
     * corresponds to the neighbors of the query point i \param[out]
     * k_sqr_distances the resultant squared distances to the neighboring
     * points, k_sqr_distances[i] corresponds to the neighbors of the query
     * point i
     */
    virtual void
    nearestKSearch(const PointCloud &cloud, const std::vector<int> &indices,
                   int k, std::vector<std::vector<int>> &k_indices,
                   std::vector<std::vector<float>> &k_sqr_distances) const;

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
    int radiusSearch(const PointT &point, double radius,
                     std::vector<int> &k_indices,
                     std::vector<float> &k_sqr_distances,
                     unsigned int max_nn = 0) const;

    /** \brief Search for the k-nearest neighbors for the given query point.
     * \param[in] cloud the point cloud data
     * \param[in] indices a vector of point cloud indices to query for nearest
     * neighbors \param[in] radius the radius of the sphere bounding all of
     * p_q's neighbors \param[out] k_indices the resultant indices of the
     * neighboring points, k_indices[i] corresponds to the neighbors of the
     * query point i \param[out] k_sqr_distances the resultant squared distances
     * to the neighboring points, k_sqr_distances[i] corresponds to the
     * neighbors of the query point i \param[in] max_nn if given, bounds the
     * maximum returned neighbors to this value
     */
    virtual void radiusSearch(const PointCloud &cloud,
                              const std::vector<int> &indices, double radius,
                              std::vector<std::vector<int>> &k_indices,
                              std::vector<std::vector<float>> &k_sqr_distances,
                              unsigned int max_nn = 0) const;

    /** \brief Provide a pointer to the point representation to use to convert
     * points into k-D vectors. \param[in] point_representation the const boost
     * shared pointer to a PointRepresentation
     */
    inline void setPointRepresentation(
        const PointRepresentationConstPtr &point_representation) {
        point_representation_ = point_representation;
        setInputCloud(
            input_,
            indices_); // re-create the tree, since point_represenation might
                       // change things such as the scaling of the point clouds.
    }

    /** \brief Get a pointer to the point representation used when converting
     * points into k-D vectors. */
    inline PointRepresentationConstPtr const getPointRepresentation() {
        return (point_representation_);
    }

  protected:
    /** \brief converts the input data to a format usable by FLANN
     */
    void convertInputToFlannMatrix();

    /** The FLANN index.
     */
    IndexPtr index_;

    /** The index creator, used to (re-) create the index when the search data
     * is passed.
     */
    FlannIndexCreatorPtr creator_;

    /** Input data in FLANN format.
     */
    MatrixPtr input_flann_;

    /** Epsilon for approximate NN search.
     */
    float eps_;
    bool input_copied_for_flann_;

    PointRepresentationConstPtr point_representation_;

    int dim_;

    std::vector<int> index_mapping_;
    bool identity_mapping_;
};
} // namespace search
} // namespace pcl

#define PCL_INSTANTIATE_FlannSearch(T)                                         \
    template class PCL_EXPORTS pcl::search::FlannSearch<T>;

#endif // PCL_SEARCH_KDTREE_H_
