/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
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
 * $Id: point_cloud.h 6566 2012-07-27 06:49:11Z rusu $
 *
 */

#ifndef PCL_POINT_CLOUD_H_
#define PCL_POINT_CLOUD_H_

#include <pcl/common/boost.h>
#include <pcl/common/eigen.h>
#include <cstddef>
#include <std_msgs/Header.h>
#include <pcl/pcl_macros.h>
#include <pcl/exceptions.h>
#include <pcl/cloud_properties.h>
#include <pcl/channel_properties.h>
#include <pcl/point_traits.h>
#include <pcl/for_each_type.h>
#include <map>

namespace pcl {
namespace detail {
struct FieldMapping {
    size_t serialized_offset;
    size_t struct_offset;
    size_t size;
};
} // namespace detail

// Forward declarations
template <typename PointT> class PointCloud;
typedef std::vector<detail::FieldMapping> MsgFieldMap;

////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Helper functor structure for copying data between an Eigen type and a
 * PointT. */
template <typename PointOutT> struct NdCopyEigenPointFunctor {
    typedef typename traits::POD<PointOutT>::type Pod;

    /** \brief Constructor
     * \param[in] p1 the input Eigen type
     * \param[out] p2 the output Point type
     */
    NdCopyEigenPointFunctor(const Eigen::VectorXf &p1, PointOutT &p2)
        : p1_(p1), p2_(reinterpret_cast<Pod &>(p2)), f_idx_(0) {}

    /** \brief Operator. Data copy happens here. */
    template <typename Key> inline void operator()() {
        // boost::fusion::at_key<Key> (p2_) = p1_[f_idx_++];
        typedef typename pcl::traits::datatype<PointOutT, Key>::type T;
        uint8_t *data_ptr = reinterpret_cast<uint8_t *>(&p2_) +
                            pcl::traits::offset<PointOutT, Key>::value;
        *reinterpret_cast<T *>(data_ptr) = static_cast<T>(p1_[f_idx_++]);
    }

  private:
    const Eigen::VectorXf &p1_;
    Pod &p2_;
    int f_idx_;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

////////////////////////////////////////////////////////////////////////////////////////////////
/** \brief Helper functor structure for copying data between an Eigen type and a
 * PointT. */
template <typename PointInT> struct NdCopyPointEigenFunctor {
    typedef typename traits::POD<PointInT>::type Pod;

    /** \brief Constructor
     * \param[in] p1 the input Point type
     * \param[out] p2 the output Eigen type
     */
    NdCopyPointEigenFunctor(const PointInT &p1, Eigen::VectorXf &p2)
        : p1_(reinterpret_cast<const Pod &>(p1)), p2_(p2), f_idx_(0) {}

    /** \brief Operator. Data copy happens here. */
    template <typename Key> inline void operator()() {
        // p2_[f_idx_++] = boost::fusion::at_key<Key> (p1_);
        typedef typename pcl::traits::datatype<PointInT, Key>::type T;
        const uint8_t *data_ptr = reinterpret_cast<const uint8_t *>(&p1_) +
                                  pcl::traits::offset<PointInT, Key>::value;
        p2_[f_idx_++] =
            static_cast<float>(*reinterpret_cast<const T *>(data_ptr));
    }

  private:
    const Pod &p1_;
    Eigen::VectorXf &p2_;
    int f_idx_;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace detail {
template <typename PointT>
boost::shared_ptr<pcl::MsgFieldMap> &getMapping(pcl::PointCloud<PointT> &p);
} // namespace detail

/** \brief PointCloud represents the base class in PCL for storing collections
 * of 3D points.
 *
 * The class is templated, which means you need to specify the type of data
 * that it should contain. For example, to create a point cloud that holds 4
 * random XYZ data points, use:
 *
 * \code
 * pcl::PointCloud<pcl::PointXYZ> cloud;
 * cloud.push_back (pcl::PointXYZ (rand (), rand (), rand ()));
 * cloud.push_back (pcl::PointXYZ (rand (), rand (), rand ()));
 * cloud.push_back (pcl::PointXYZ (rand (), rand (), rand ()));
 * cloud.push_back (pcl::PointXYZ (rand (), rand (), rand ()));
 * \endcode
 *
 * The PointCloud class contains the following elements:
 *   - \b width - specifies the width of the point cloud dataset in the number
 * of points. WIDTH has two meanings:
 *     - it can specify the total number of points in the cloud (equal with
 * POINTS see below) for unorganized datasets;
 *     - it can specify the width (total number of points in a row) of an
 * organized point cloud dataset. \a Mandatory.
 *   - \b height - specifies the height of the point cloud dataset in the number
 * of points. HEIGHT has two meanings:
 *     - it can specify the height (total number of rows) of an organized point
 * cloud dataset;
 *     - it is set to 1 for unorganized datasets (thus used to check whether a
 * dataset is organized or not). \a Mandatory.
 *   - \b points - the data array where all points of type <b>PointT</b> are
 * stored. \a Mandatory.
 *
 *   - \b is_dense - specifies if all the data in <b>points</b> is finite
 * (true), or whether it might contain Inf/NaN values (false). \a Mandatory.
 *
 *   - \b sensor_origin_ - specifies the sensor acquisition pose
 * (origin/translation). \a Optional.
 *   - \b sensor_orientation_ - specifies the sensor acquisition pose
 * (rotation). \a Optional.
 *
 * \author Patrick Mihelich, Radu B. Rusu
 */
template <typename PointT> class PointCloud {
  public:
    /** \brief Default constructor. Sets \ref is_dense to true, \ref width
     * and \ref height to 0, and the \ref sensor_origin_ and \ref
     * sensor_orientation_ to identity.
     */
    PointCloud()
        : header(), points(), width(0), height(0), is_dense(true),
          sensor_origin_(Eigen::Vector4f::Zero()),
          sensor_orientation_(Eigen::Quaternionf::Identity()), mapping_() {}

    /** \brief Copy constructor (needed by compilers such as Intel C++)
     * \param[in] pc the cloud to copy into this
     */
    PointCloud(PointCloud<PointT> &pc)
        : header(), points(), width(0), height(0), is_dense(true),
          sensor_origin_(Eigen::Vector4f::Zero()),
          sensor_orientation_(Eigen::Quaternionf::Identity()), mapping_() {
        *this = pc;
    }

    /** \brief Copy constructor (needed by compilers such as Intel C++)
     * \param[in] pc the cloud to copy into this
     */
    PointCloud(const PointCloud<PointT> &pc)
        : header(), points(), width(0), height(0), is_dense(true),
          sensor_origin_(Eigen::Vector4f::Zero()),
          sensor_orientation_(Eigen::Quaternionf::Identity()), mapping_() {
        *this = pc;
    }

    /** \brief Copy constructor from point cloud subset
     * \param[in] pc the cloud to copy into this
     * \param[in] indices the subset to copy
     */
    PointCloud(const PointCloud<PointT> &pc, const std::vector<int> &indices)
        : header(pc.header), points(indices.size()), width(indices.size()),
          height(1), is_dense(pc.is_dense), sensor_origin_(pc.sensor_origin_),
          sensor_orientation_(pc.sensor_orientation_), mapping_() {
        // Copy the obvious
        assert(indices.size() <= pc.size());
        for (size_t i = 0; i < indices.size(); i++)
            points[i] = pc.points[indices[i]];
    }

    /** \brief Allocate constructor from point cloud subset
     * \param[in] width_ the cloud width
     * \param[in] height_ the cloud height
     * \param[in] value_ default value
     */
    PointCloud(uint32_t width_, uint32_t height_,
               const PointT &value_ = PointT())
        : header(), points(width_ * height_, value_), width(width_),
          height(height_), is_dense(true),
          sensor_origin_(Eigen::Vector4f::Zero()),
          sensor_orientation_(Eigen::Quaternionf::Identity()), mapping_() {}

    /** \brief Destructor. */
    virtual ~PointCloud() {}

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Add a point cloud to the current cloud.
     * \param[in] rhs the cloud to add to the current cloud
     * \return the new cloud as a concatenation of the current cloud and the new
     * given cloud
     */
    inline PointCloud &operator+=(const PointCloud &rhs) {
        // Make the resultant point cloud take the newest stamp
        if (rhs.header.stamp > header.stamp)
            header.stamp = rhs.header.stamp;

        size_t nr_points = points.size();
        points.resize(nr_points + rhs.points.size());
        for (size_t i = nr_points; i < points.size(); ++i)
            points[i] = rhs.points[i - nr_points];

        width = static_cast<uint32_t>(points.size());
        height = 1;
        if (rhs.is_dense && is_dense)
            is_dense = true;
        else
            is_dense = false;
        return (*this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Add a point cloud to another cloud.
     * \param[in] rhs the cloud to add to the current cloud
     * \return the new cloud as a concatenation of the current cloud and the new
     * given cloud
     */
    inline const PointCloud operator+(const PointCloud &rhs) {
        return (PointCloud(*this) += rhs);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline const PointT &at(int column, int row) const {
        if (this->height > 1)
            return (points.at(row * this->width + column));
        else
            throw IsNotDenseException(
                "Can't use 2D indexing with a unorganized point cloud");
    }

    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline PointT &at(int column, int row) {
        if (this->height > 1)
            return (points.at(row * this->width + column));
        else
            throw IsNotDenseException(
                "Can't use 2D indexing with a unorganized point cloud");
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline const PointT &operator()(size_t column, size_t row) const {
        return (points[row * this->width + column]);
    }

    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline PointT &operator()(size_t column, size_t row) {
        return (points[row * this->width + column]);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Return whether a dataset is organized (e.g., arranged in a
     * structured grid). \note The height value must be different than 1 for a
     * dataset to be organized.
     */
    inline bool isOrganized() const { return (height != 1); }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Return an Eigen MatrixXf (assumes float values) mapped to the
     * specified dimensions of the PointCloud. \anchor getMatrixXfMap \note This
     * method is for advanced users only! Use with care!
     *
     * \attention Since 1.4.0, Eigen matrices are forced to Row Major to
     * increase the efficiency of the algorithms in PCL This means that the
     * behavior of getMatrixXfMap changed, and is now correctly mapping 1-1 with
     * a PointCloud structure, that is: number of points in a cloud = rows in a
     * matrix, number of point dimensions = columns in a matrix
     *
     * \param[in] dim the number of dimensions to consider for each point
     * \param[in] stride the number of values in each point (will be the number
     * of values that separate two of the columns) \param[in] offset the number
     * of dimensions to skip from the beginning of each point (stride = offset +
     * dim + x, where x is the number of dimensions to skip from the end of each
     * point) \note for getting only XYZ coordinates out of PointXYZ use dim=3,
     * stride=4 and offset=0 due to the alignment. \attention PointT types are
     * most of the time aligned, so the offsets are not continuous!
     */
    inline Eigen::Map<Eigen::MatrixXf, Eigen::Aligned, Eigen::OuterStride<>>
    getMatrixXfMap(int dim, int stride, int offset) {
        if (Eigen::MatrixXf::Flags & Eigen::RowMajorBit)
            return (Eigen::Map<Eigen::MatrixXf, Eigen::Aligned,
                               Eigen::OuterStride<>>(
                reinterpret_cast<float *>(&points[0]) + offset, points.size(),
                dim, Eigen::OuterStride<>(stride)));
        else
            return (Eigen::Map<Eigen::MatrixXf, Eigen::Aligned,
                               Eigen::OuterStride<>>(
                reinterpret_cast<float *>(&points[0]) + offset, dim,
                points.size(), Eigen::OuterStride<>(stride)));
    }

    /** \brief Return an Eigen MatrixXf (assumes float values) mapped to the
     * specified dimensions of the PointCloud. \anchor getMatrixXfMap \note This
     * method is for advanced users only! Use with care!
     *
     * \attention Since 1.4.0, Eigen matrices are forced to Row Major to
     * increase the efficiency of the algorithms in PCL This means that the
     * behavior of getMatrixXfMap changed, and is now correctly mapping 1-1 with
     * a PointCloud structure, that is: number of points in a cloud = rows in a
     * matrix, number of point dimensions = columns in a matrix
     *
     * \param[in] dim the number of dimensions to consider for each point
     * \param[in] stride the number of values in each point (will be the number
     * of values that separate two of the columns) \param[in] offset the number
     * of dimensions to skip from the beginning of each point (stride = offset +
     * dim + x, where x is the number of dimensions to skip from the end of each
     * point) \note for getting only XYZ coordinates out of PointXYZ use dim=3,
     * stride=4 and offset=0 due to the alignment. \attention PointT types are
     * most of the time aligned, so the offsets are not continuous!
     */
    inline const Eigen::Map<const Eigen::MatrixXf, Eigen::Aligned,
                            Eigen::OuterStride<>>
    getMatrixXfMap(int dim, int stride, int offset) const {
        if (Eigen::MatrixXf::Flags & Eigen::RowMajorBit)
            return (Eigen::Map<const Eigen::MatrixXf, Eigen::Aligned,
                               Eigen::OuterStride<>>(
                reinterpret_cast<float *>(&points[0]) + offset, points.size(),
                dim, Eigen::OuterStride<>(stride)));
        else
            return (Eigen::Map<const Eigen::MatrixXf, Eigen::Aligned,
                               Eigen::OuterStride<>>(
                reinterpret_cast<float *>(&points[0]) + offset, dim,
                points.size(), Eigen::OuterStride<>(stride)));
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Return an Eigen MatrixXf (assumes float values) mapped to the
     * PointCloud. \note This method is for advanced users only! Use with care!
     * \attention PointT types are most of the time aligned, so the offsets are
     * not continuous! See \ref getMatrixXfMap for more information.
     */
    inline Eigen::Map<Eigen::MatrixXf, Eigen::Aligned, Eigen::OuterStride<>>
    getMatrixXfMap() {
        return (getMatrixXfMap(sizeof(PointT) / sizeof(float),
                               sizeof(PointT) / sizeof(float), 0));
    }

    /** \brief Return an Eigen MatrixXf (assumes float values) mapped to the
     * PointCloud. \note This method is for advanced users only! Use with care!
     * \attention PointT types are most of the time aligned, so the offsets are
     * not continuous! See \ref getMatrixXfMap for more information.
     */
    inline const Eigen::Map<const Eigen::MatrixXf, Eigen::Aligned,
                            Eigen::OuterStride<>>
    getMatrixXfMap() const {
        return (getMatrixXfMap(sizeof(PointT) / sizeof(float),
                               sizeof(PointT) / sizeof(float), 0));
    }

    /** \brief The point cloud header. It contains information about the
     * acquisition time. */
    std_msgs::Header header;

    /** \brief The point data. */
    std::vector<PointT, Eigen::aligned_allocator<PointT>> points;

    /** \brief The point cloud width (if organized as an image-structure). */
    uint32_t width;
    /** \brief The point cloud height (if organized as an image-structure). */
    uint32_t height;

    /** \brief True if no points are invalid (e.g., have NaN or Inf values). */
    bool is_dense;

    /** \brief Sensor acquisition pose (origin/translation). */
    Eigen::Vector4f sensor_origin_;
    /** \brief Sensor acquisition pose (rotation). */
    Eigen::Quaternionf sensor_orientation_;

    typedef PointT
        PointType; // Make the template class available from the outside
    typedef std::vector<PointT, Eigen::aligned_allocator<PointT>> VectorType;
    typedef std::vector<PointCloud<PointT>,
                        Eigen::aligned_allocator<PointCloud<PointT>>>
        CloudVectorType;
    typedef boost::shared_ptr<PointCloud<PointT>> Ptr;
    typedef boost::shared_ptr<const PointCloud<PointT>> ConstPtr;

    // iterators
    typedef typename VectorType::iterator iterator;
    typedef typename VectorType::const_iterator const_iterator;
    inline iterator begin() { return (points.begin()); }
    inline iterator end() { return (points.end()); }
    inline const_iterator begin() const { return (points.begin()); }
    inline const_iterator end() const { return (points.end()); }

    // capacity
    inline size_t size() const { return (points.size()); }
    inline void reserve(size_t n) { points.reserve(n); }
    inline bool empty() const { return points.empty(); }

    /** \brief Resize the cloud
     * \param[in] n the new cloud size
     */
    inline void resize(size_t n) {
        points.resize(n);
        if (width * height != n) {
            width = static_cast<uint32_t>(n);
            height = 1;
        }
    }

    // element access
    inline const PointT &operator[](size_t n) const { return (points[n]); }
    inline PointT &operator[](size_t n) { return (points[n]); }
    inline const PointT &at(size_t n) const { return (points.at(n)); }
    inline PointT &at(size_t n) { return (points.at(n)); }
    inline const PointT &front() const { return (points.front()); }
    inline PointT &front() { return (points.front()); }
    inline const PointT &back() const { return (points.back()); }
    inline PointT &back() { return (points.back()); }

    /** \brief Insert a new point in the cloud, at the end of the container.
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] pt the point to insert
     */
    inline void push_back(const PointT &pt) {
        points.push_back(pt);
        width = static_cast<uint32_t>(points.size());
        height = 1;
    }

    /** \brief Insert a new point in the cloud, given an iterator.
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] position where to insert the point \param[in] pt
     * the point to insert \return returns the new position iterator
     */
    inline iterator insert(iterator position, const PointT &pt) {
        iterator it = points.insert(position, pt);
        width = static_cast<uint32_t>(points.size());
        height = 1;
        return (it);
    }

    /** \brief Insert a new point in the cloud N times, given an iterator.
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] position where to insert the point \param[in] n
     * the number of times to insert the point \param[in] pt the point to insert
     */
    inline void insert(iterator position, size_t n, const PointT &pt) {
        points.insert(position, n, pt);
        width = static_cast<uint32_t>(points.size());
        height = 1;
    }

    /** \brief Insert a new range of points in the cloud, at a certain position.
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] position where to insert the data \param[in]
     * first where to start inserting the points from \param[in] last where to
     * stop inserting the points from
     */
    template <class InputIterator>
    inline void insert(iterator position, InputIterator first,
                       InputIterator last) {
        points.insert(position, first, last);
        width = static_cast<uint32_t>(points.size());
        height = 1;
    }

    /** \brief Erase a point in the cloud.
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] position what data point to erase \return returns
     * the new position iterator
     */
    inline iterator erase(iterator position) {
        iterator it = points.erase(position);
        width = static_cast<uint32_t>(points.size());
        height = 1;
        return (it);
    }

    /** \brief Erase a set of points given by a (first, last) iterator pair
     * \note This breaks the organized structure of the cloud by setting the
     * height to 1! \param[in] first where to start erasing points from
     * \param[in] last where to stop erasing points from
     * \return returns the new position iterator
     */
    inline iterator erase(iterator first, iterator last) {
        iterator it = points.erase(first, last);
        width = static_cast<uint32_t>(points.size());
        height = 1;
        return (it);
    }

    /** \brief Swap a point cloud with another cloud.
     * \param[in,out] rhs point cloud to swap this with
     */
    inline void swap(PointCloud<PointT> &rhs) {
        this->points.swap(rhs.points);
        std::swap(width, rhs.width);
        std::swap(height, rhs.height);
        std::swap(is_dense, rhs.is_dense);
        std::swap(sensor_origin_, rhs.sensor_origin_);
        std::swap(sensor_orientation_, rhs.sensor_orientation_);
    }

    /** \brief Removes all points in a cloud and sets the width and height to 0.
     */
    inline void clear() {
        points.clear();
        width = 0;
        height = 0;
    }

    /** \brief Copy the cloud to the heap and return a smart pointer
     * Note that deep copy is performed, so avoid using this function on
     * non-empty clouds. The changes of the returned cloud are not mirrored back
     * to this one. \return shared pointer to the copy of the cloud
     */
    inline Ptr makeShared() const { return Ptr(new PointCloud<PointT>(*this)); }

  protected:
    // This is motivated by ROS integration. Users should not need to access
    // mapping_.
    boost::shared_ptr<MsgFieldMap> mapping_;

    friend boost::shared_ptr<MsgFieldMap> &
    detail::getMapping<PointT>(pcl::PointCloud<PointT> &p);

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/** \brief PointCloud specialization for Eigen matrices. <b>For advanced users
 * only!</b>
 *
 * \attention
 * As opposed to the generic PointCloud<T> implementation where the point
 * cloud data is stored in a std::vector<T>, this specialization uses an
 * Eigen::MatrixXf. This means that this specialization is \b NOT suitable
 * for other types of data other than floats, and should therefore be used
 * with extreme caution!
 *
 * \note
 * Similar to PointCloud<T> where each data point represents a new element
 * in the vector, we are storing each data point on a separate row in our
 * MatrixXf, in a row-major format. This means that the data is aligned in
 * memory in a row-major format. Please check that PCL is compiled using
 * \b -DEIGEN_DEFAULT_TO_ROW_MAJOR.
 *
 * \author Radu B. Rusu
 */
template <> class PointCloud<Eigen::MatrixXf> {
  public:
    /** \brief Default constructor. Sets \ref is_dense to true, \ref width
     * and \ref height to 0.
     */
    PointCloud()
        : properties(), points(Eigen::MatrixXf(0, 0)), channels(), width(0),
          height(0), is_dense(true) {}

    /** \brief Copy constructor (needed by compilers such as Intel C++)
     * \param[in] pc the cloud to copy into this
     */
    PointCloud(PointCloud<Eigen::MatrixXf> &pc)
        : properties(), points(Eigen::MatrixXf(0, 0)), channels(), width(0),
          height(0), is_dense(true) {
        *this = pc;
    }

    /** \brief Copy conversion constructor (needed by compilers such as Intel
     * C++) \param[in] pc the cloud<T> to copy into this
     */
    template <typename PointT>
    PointCloud(PointCloud<PointT> &pc)
        : properties(), points(Eigen::MatrixXf(0, 0)), channels(),
          width(pc.width), height(pc.height), is_dense(pc.is_dense) {
        // Copy the obvious
        properties.acquisition_time = pc.header.stamp;
        properties.sensor_origin = pc.sensor_origin_; //.head<3> ();
        properties.sensor_orientation = pc.sensor_orientation_;

        typedef typename pcl::traits::fieldList<PointT>::type FieldList;
        // Copy the fields
        pcl::for_each_type<FieldList>(
            CopyFieldsChannelProperties<PointT>(channels));

        // Resize the array
        points.resize(pc.points.size(), boost::mpl::size<FieldList>::value);

        for (size_t cp = 0; cp < pc.points.size(); ++cp)
            pcl::for_each_type<FieldList>(
                NdCopyPointEigenFunctor<PointT, Eigen::MatrixXf::RowXpr>(
                    pc.points[cp], points.row(cp)));
    }

    /** \brief Copy constructor (needed by compilers such as Intel C++)
     * \param[in] pc the cloud to copy into this
     */
    PointCloud(const PointCloud<Eigen::MatrixXf> &pc)
        : properties(), points(Eigen::MatrixXf(0, 0)), channels(), width(0),
          height(0), is_dense(true) {
        *this = pc;
    }

    /** \brief Copy conversion constructor (needed by compilers such as Intel
     * C++) \param[in] pc the cloud<T> to copy into this
     */
    template <typename PointT>
    PointCloud(const PointCloud<PointT> &pc)
        : properties(), points(Eigen::MatrixXf(0, 0)), channels(),
          width(pc.width), height(pc.height), is_dense(pc.is_dense) {
        // Copy the obvious
        properties.acquisition_time = pc.header.stamp;
        properties.sensor_origin = pc.sensor_origin_; //.head<3> ();
        properties.sensor_orientation = pc.sensor_orientation_;

        typedef typename pcl::traits::fieldList<PointT>::type FieldList;

        // Copy the fields
        pcl::for_each_type<FieldList>(
            CopyFieldsChannelProperties<PointT>(channels));

        // Resize the array
        points.resize(pc.points.size(), boost::mpl::size<FieldList>::value);

        for (size_t cp = 0; cp < pc.points.size(); ++cp)
            pcl::for_each_type<FieldList>(
                NdCopyPointEigenFunctor<PointT, Eigen::MatrixXf::RowXpr>(
                    pc.points[cp], points.row(cp)));
    }

    /** \brief Copy constructor from point cloud subset
     * \param[in] pc the cloud to copy into this
     * \param[in] indices the subset to copy
     */
    PointCloud(const PointCloud &pc, const std::vector<int> &indices)
        : properties(pc.properties),
          points(Eigen::MatrixXf(indices.size(), pc.points.cols())),
          channels(pc.channels), width(static_cast<uint32_t>(indices.size())),
          height(1), is_dense(pc.is_dense) {
        // Copy the obvious
        assert(static_cast<int>(indices.size()) <= pc.points.rows());
        for (size_t i = 0; i < indices.size(); i++)
            points.row(i) = pc.points.row(indices[i]);
    }

    /** \brief Initialize size constructor for \b organized point cloud
     * datasets. Sets \ref is_dense to true. \param[in] _width the number of
     * points in the cloud per row (if organized set to the width of the image,
     * else set to the total number of points) \param[in] _height the number of
     * points in the cloud per column (if organized set to the height of the
     * image, else set to 1) \param[in] _dim the number of dimensions that each
     * point entry will have (e.g., 3=3D, 6=6D)
     */
    inline PointCloud(uint32_t _width, uint32_t _height, uint32_t _dim)
        : properties(), points(Eigen::MatrixXf(_width * _height, _dim)),
          channels(), width(_width), height(_height), is_dense(true) {}

    /** \brief Initialize size constructor for \b unorganized point cloud
     * datasets. Sets \ref is_dense to true. \param[in] _num_points the number
     * of points in the cloud. Sets \ref width to \a _num_points and \ref height
     * to 1. \param[in] _dim the number of dimensions that each point entry will
     * have (e.g., 3=3D, 6=6D)
     */
    inline PointCloud(uint32_t _num_points, uint32_t _dim)
        : properties(), points(Eigen::MatrixXf(_num_points, _dim)), channels(),
          width(_num_points), height(1), is_dense(true) {}

    /** Destructor. */
    virtual ~PointCloud() {}

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Add a point cloud to the current cloud.
     * \param[in] rhs the cloud to add to the current cloud
     * \return the new cloud as a concatenation of the current cloud and the new
     * given cloud
     */
    inline PointCloud &operator+=(const PointCloud &rhs) {
        if (rhs.properties.acquisition_time > properties.acquisition_time)
            properties.acquisition_time = rhs.properties.acquisition_time;

        properties.sensor_origin = Eigen::Vector4f::Zero();
        properties.sensor_orientation = Eigen::Quaternionf::Identity();

        int nr_points = static_cast<int>(points.rows());
        points.resize(nr_points + rhs.points.rows(), points.cols());
        for (int i = nr_points; i < points.rows(); ++i)
            points.row(i) = rhs.points.row(i - nr_points);

        channels = rhs.channels;
        width = static_cast<uint32_t>(points.rows());
        height = 1;
        if (rhs.is_dense && is_dense)
            is_dense = true;
        else
            is_dense = false;
        return (*this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Add a point cloud to another cloud.
     * \param[in] rhs the cloud to add to the current cloud
     * \return the new cloud as a concatenation of the current cloud and the new
     * given cloud
     */
    inline const PointCloud operator+(const PointCloud &rhs) {
        return (PointCloud(*this) += rhs);
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline Eigen::Map<Eigen::VectorXf> at(int column, int row) {
        if (height > 1)
            return (Eigen::VectorXf::Map(&points(row * width + column, 0),
                                         points.cols()));
        else
            throw IsNotDenseException(
                "Can't use 2D indexing with a unorganized point cloud");
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Obtain the point given by the (column, row) coordinates. Only
     * works on organized datasets (those that have height != 1). \param[in]
     * column the column coordinate \param[in] row the row coordinate
     */
    inline Eigen::Map<Eigen::VectorXf> operator()(int column, int row) {
        return (Eigen::VectorXf::Map(&points(row * width + column, 0),
                                     points.cols()));
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Return whether a dataset is organized (e.g., arranged in a
     * structured grid). \note The height value must be different than 1 for a
     * dataset to be organized.
     */
    inline bool isOrganized() const { return (height != 1); }

    ////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Return whether a dataset is empty (i.e., has no points) */
    inline bool empty() const { return (points.rows() == 0); }

    /** \brief A list of optional point cloud properties. See \ref
     * CloudProperties for more information. */
    pcl::CloudProperties properties;

    /** \brief The point data. */
    Eigen::MatrixXf points;

    /** \brief The channel data information. We need the entries to be ordered
     * here. */
    std::map<std::string, pcl::ChannelProperties> channels;

    /** \brief The point cloud width (if organized as an image-structure). */
    uint32_t width;
    /** \brief The point cloud height (if organized as an image-structure). */
    uint32_t height;

    /** \brief True if no points are invalid (e.g., have NaN or Inf values). */
    bool is_dense;

    typedef boost::shared_ptr<PointCloud<Eigen::MatrixXf>> Ptr;
    typedef boost::shared_ptr<const PointCloud<Eigen::MatrixXf>> ConstPtr;

    inline size_t size() const { return (points.rows()); }

    /** \brief Swap a point cloud with another cloud.
     * \param[in,out] rhs point cloud to swap this with
     */
    inline void swap(PointCloud<Eigen::MatrixXf> &rhs) {
        std::swap(points, rhs.points);
        std::swap(width, rhs.width);
        std::swap(height, rhs.height);
        std::swap(is_dense, rhs.is_dense);
        std::swap(properties, rhs.properties);
        std::swap(channels, rhs.channels);
    }

    /** \brief Removes all points in a cloud and sets the width and height to 0.
     */
    inline void clear() {
        points.resize(0, 0);
        width = 0;
        height = 0;
    }

    /** \brief Copy the cloud to the heap and return a smart pointer
     * Note that deep copy is performed, so avoid using this function on
     * non-empty clouds. The changes of the returned cloud are not mirrored back
     * to this one. \return shared pointer to the copy of the cloud
     */
    inline Ptr makeShared() {
        return Ptr(new PointCloud<Eigen::MatrixXf>(*this));
    }

    /** \brief Copy the cloud to the heap and return a constant smart pointer
     * Note that deep copy is performed, so avoid using this function on
     * non-empty clouds. \return const shared pointer to the copy of the cloud
     */
    inline ConstPtr makeShared() const {
        return ConstPtr(new PointCloud<Eigen::MatrixXf>(*this));
    }

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  private:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Helper functor structure for copying data between an Eigen type
     * and a PointT. */
    template <typename PointOutT, typename PointInT>
    struct NdCopyEigenPointFunctor {
        typedef typename traits::POD<PointOutT>::type Pod;

        /** \brief Constructor
         * \param[in] p1 the input Eigen type
         * \param[out] p2 the output Point type
         */
        NdCopyEigenPointFunctor(const PointInT p1, PointOutT &p2)
            : p1_(p1), p2_(reinterpret_cast<Pod &>(p2)), f_idx_(0) {}

        /** \brief Operator. Data copy happens here. */
        template <typename Key> inline void operator()() {
            // boost::fusion::at_key<Key> (p2_) = p1_[f_idx_++];
            typedef typename pcl::traits::datatype<PointOutT, Key>::type T;
            uint8_t *data_ptr = reinterpret_cast<uint8_t *>(&p2_) +
                                pcl::traits::offset<PointOutT, Key>::value;
            *reinterpret_cast<T *>(data_ptr) = p1_[f_idx_++];
        }

      private:
        const PointInT p1_;
        Pod &p2_;
        int f_idx_;

      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Helper functor structure for copying data between an Eigen type
     * and a PointT. */
    template <typename PointInT, typename PointOutT>
    struct NdCopyPointEigenFunctor {
        typedef typename traits::POD<PointInT>::type Pod;

        /** \brief Constructor
         * \param[in] p1 the input Point type
         * \param[out] p2 the output Eigen type
         */
        NdCopyPointEigenFunctor(const PointInT &p1, PointOutT p2)
            : p1_(reinterpret_cast<const Pod &>(p1)), p2_(p2), f_idx_(0) {}

        /** \brief Operator. Data copy happens here. */
        template <typename Key> inline void operator()() {
            // p2_[f_idx_++] = boost::fusion::at_key<Key> (p1_);
            typedef typename pcl::traits::datatype<PointInT, Key>::type T;
            const uint8_t *data_ptr = reinterpret_cast<const uint8_t *>(&p1_) +
                                      pcl::traits::offset<PointInT, Key>::value;
            p2_[f_idx_++] = *reinterpret_cast<const T *>(data_ptr);
        }

      private:
        const Pod &p1_;
        PointOutT p2_;
        int f_idx_;

      public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /** \brief Helper functor structure for copying channel information */
    template <typename T> struct CopyFieldsChannelProperties {
        /** \brief Constructor
         * \param[out] channels the map of ChannelProperties
         */
        CopyFieldsChannelProperties(
            std::map<std::string, pcl::ChannelProperties> &channels)
            : channels_(channels) {}

        /** \brief Operator. Data copy happens here. */
        template <typename U> inline void operator()() {
            // boost::fusion::at_key<Key> (p2_) = p1_[f_idx_++];
            std::string name = pcl::traits::name<T, U>::value;
            channels_[name].name = name;
            channels_[name].offset = pcl::traits::offset<T, U>::value;
            uint8_t datatype = pcl::traits::datatype<T, U>::value;
            channels_[name].datatype = datatype;
            int count = pcl::traits::datatype<T, U>::size;
            channels_[name].count = count;
            switch (datatype) {
            case sensor_msgs::PointField::INT8:
            case sensor_msgs::PointField::UINT8: {
                channels_[name].size = count;
                break;
            }

            case sensor_msgs::PointField::INT16:
            case sensor_msgs::PointField::UINT16: {
                channels_[name].size = count * 2;
                break;
            }

            case sensor_msgs::PointField::INT32:
            case sensor_msgs::PointField::UINT32:
            case sensor_msgs::PointField::FLOAT32: {
                channels_[name].size = count * 4;
                break;
            }

            case sensor_msgs::PointField::FLOAT64: {
                channels_[name].size = count * 8;
                break;
            }
            }
        }

      private:
        std::map<std::string, pcl::ChannelProperties> &channels_;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////
namespace detail {
template <typename PointT>
boost::shared_ptr<pcl::MsgFieldMap> &getMapping(pcl::PointCloud<PointT> &p) {
    return (p.mapping_);
}
} // namespace detail

template <typename PointT>
std::ostream &operator<<(std::ostream &s, const pcl::PointCloud<PointT> &p) {
    s << "points[]: " << p.points.size() << std::endl;
    s << "width: " << p.width << std::endl;
    s << "height: " << p.height << std::endl;
    s << "is_dense: " << p.is_dense << std::endl;
    return (s);
}
} // namespace pcl

#endif //#ifndef PCL_POINT_CLOUD_H_
