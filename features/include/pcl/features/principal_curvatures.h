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
 * $Id: principal_curvatures.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_PRINCIPAL_CURVATURES_H_
#define PCL_PRINCIPAL_CURVATURES_H_

#include <pcl/features/eigen.h>
#include <pcl/features/feature.h>

namespace pcl {
/** \brief PrincipalCurvaturesEstimation estimates the directions (eigenvectors)
 * and magnitudes (eigenvalues) of principal surface curvatures for a given
 * point cloud dataset containing points and normals.
 *
 * The recommended PointOutT is pcl::PrincipalCurvatures.
 *
 * \note The code is stateful as we do not expect this class to be multicore
 * parallelized. Please look at \ref NormalEstimationOMP for an example on how
 * to extend this to parallel implementations.
 *
 * \author Radu B. Rusu, Jared Glover
 * \ingroup features
 */
template <typename PointInT, typename PointNT,
          typename PointOutT = pcl::PrincipalCurvatures>
class PrincipalCurvaturesEstimation
    : public FeatureFromNormals<PointInT, PointNT, PointOutT> {
  public:
    using Feature<PointInT, PointOutT>::feature_name_;
    using Feature<PointInT, PointOutT>::getClassName;
    using Feature<PointInT, PointOutT>::indices_;
    using Feature<PointInT, PointOutT>::k_;
    using Feature<PointInT, PointOutT>::search_parameter_;
    using Feature<PointInT, PointOutT>::surface_;
    using Feature<PointInT, PointOutT>::input_;
    using FeatureFromNormals<PointInT, PointNT, PointOutT>::normals_;

    typedef typename Feature<PointInT, PointOutT>::PointCloudOut PointCloudOut;
    typedef pcl::PointCloud<PointInT> PointCloudIn;

    /** \brief Empty constructor. */
    PrincipalCurvaturesEstimation()
        : projected_normals_(), xyz_centroid_(Eigen::Vector3f::Zero()),
          demean_(Eigen::Vector3f::Zero()),
          covariance_matrix_(Eigen::Matrix3f::Zero()),
          eigenvector_(Eigen::Vector3f::Zero()),
          eigenvalues_(Eigen::Vector3f::Zero()) {
        feature_name_ = "PrincipalCurvaturesEstimation";
    };

    /** \brief Perform Principal Components Analysis (PCA) on the point normals
     * of a surface patch in the tangent plane of the given point normal, and
     * return the principal curvature (eigenvector of the max eigenvalue), along
     * with both the max (pc1) and min (pc2) eigenvalues \param[in] normals the
     * point cloud normals \param[in] p_idx the query point at which the
     * least-squares plane was estimated \param[in] indices the point cloud
     * indices that need to be used \param[out] pcx the principal curvature X
     * direction \param[out] pcy the principal curvature Y direction \param[out]
     * pcz the principal curvature Z direction \param[out] pc1 the max
     * eigenvalue of curvature \param[out] pc2 the min eigenvalue of curvature
     */
    void
    computePointPrincipalCurvatures(const pcl::PointCloud<PointNT> &normals,
                                    int p_idx, const std::vector<int> &indices,
                                    float &pcx, float &pcy, float &pcz,
                                    float &pc1, float &pc2);

  protected:
    /** \brief Estimate the principal curvature (eigenvector of the max
     * eigenvalue), along with both the max (pc1) and min (pc2) eigenvalues for
     * all points given in <setInputCloud (), setIndices ()> using the surface
     * in setSearchSurface () and the spatial locator in setSearchMethod ()
     * \param[out] output the resultant point cloud model dataset that contains
     * the principal curvature estimates
     */
    void computeFeature(PointCloudOut &output);

  private:
    /** \brief A pointer to the input dataset that contains the point normals of
     * the XYZ dataset. */
    std::vector<Eigen::Vector3f> projected_normals_;

    /** \brief SSE aligned placeholder for the XYZ centroid of a surface patch.
     */
    Eigen::Vector3f xyz_centroid_;

    /** \brief Temporary point placeholder. */
    Eigen::Vector3f demean_;

    /** \brief Placeholder for the 3x3 covariance matrix at each surface patch.
     */
    EIGEN_ALIGN16 Eigen::Matrix3f covariance_matrix_;

    /** \brief SSE aligned eigenvectors placeholder for a covariance matrix. */
    Eigen::Vector3f eigenvector_;
    /** \brief eigenvalues placeholder for a covariance matrix. */
    Eigen::Vector3f eigenvalues_;

    /** \brief Make the computeFeature (&Eigen::MatrixXf); inaccessible from
     * outside the class \param[out] output the output point cloud
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &) {}
};

/** \brief PrincipalCurvaturesEstimation estimates the directions (eigenvectors)
 * and magnitudes (eigenvalues) of principal surface curvatures for a given
 * point cloud dataset containing points and normals.
 *
 * \note The code is stateful as we do not expect this class to be multicore
 * parallelized. Please look at \ref NormalEstimationOMP for an example on how
 * to extend this to parallel implementations.
 *
 * \author Radu B. Rusu, Jared Glover
 * \ingroup features
 */
template <typename PointInT, typename PointNT>
class PrincipalCurvaturesEstimation<PointInT, PointNT, Eigen::MatrixXf>
    : public PrincipalCurvaturesEstimation<PointInT, PointNT,
                                           pcl::PrincipalCurvatures> {
  public:
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::indices_;
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::k_;
    using PrincipalCurvaturesEstimation<
        PointInT, PointNT, pcl::PrincipalCurvatures>::search_parameter_;
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::surface_;
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::compute;
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::input_;
    using PrincipalCurvaturesEstimation<PointInT, PointNT,
                                        pcl::PrincipalCurvatures>::normals_;

  private:
    /** \brief Estimate the principal curvature (eigenvector of the max
     * eigenvalue), along with both the max (pc1) and min (pc2) eigenvalues for
     * all points given in <setInputCloud (), setIndices ()> using the surface
     * in setSearchSurface () and the spatial locator in setSearchMethod ()
     * \param[out] output the resultant point cloud model dataset that contains
     * the principal curvature estimates
     */
    void computeFeatureEigen(pcl::PointCloud<Eigen::MatrixXf> &output);

    /** \brief Make the compute (&PointCloudOut); inaccessible from outside the
     * class \param[out] output the output point cloud
     */
    void compute(pcl::PointCloud<pcl::Normal> &) {}
};
} // namespace pcl

#endif //#ifndef PCL_PRINCIPAL_CURVATURES_H_
