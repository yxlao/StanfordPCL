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
 */

#ifndef PCL_ISS_KEYPOINT3D_IMPL_H_
#define PCL_ISS_KEYPOINT3D_IMPL_H_

#include <pcl/features/boundary.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/integral_image_normal.h>

#include <pcl/keypoints/iss_3d.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setSalientRadius(
    double salient_radius) {
    salient_radius_ = salient_radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setNonMaxRadius(
    double non_max_radius) {
    non_max_radius_ = non_max_radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setNormalRadius(
    double normal_radius) {
    normal_radius_ = normal_radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setBorderRadius(
    double border_radius) {
    border_radius_ = border_radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setThreshold21(
    double gamma_21) {
    gamma_21_ = gamma_21;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setThreshold32(
    double gamma_32) {
    gamma_32_ = gamma_32;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setMinNeighbors(
    int min_neighbors) {
    min_neighbors_ = min_neighbors;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::setNormals(
    const PointCloudNConstPtr &normals) {
    normals_ = normals;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
bool *pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::getBoundaryPoints(
    PointCloudIn &input, double border_radius) {
    bool *edge_points = new bool[input.size()];

    Eigen::Vector4f u = Eigen::Vector4f::Zero();
    Eigen::Vector4f v = Eigen::Vector4f::Zero();

    pcl::BoundaryEstimation<PointInT, NormalT, pcl::Boundary>
        boundary_estimator;

    size_t index;
#ifdef _OPENMP
#pragma omp parallel for private(u, v) num_threads(threads_)
#endif
    for (index = 0; index < input.points.size(); index++) {
        edge_points[index] = false;

        std::vector<int> nn_indices;
        std::vector<float> nn_distances;
        int n_neighbors;

        this->searchForNeighbors(index, border_radius, nn_indices,
                                 nn_distances);

        n_neighbors = nn_indices.size();

        if (n_neighbors < min_neighbors_)
            continue;

        boundary_estimator.getCoordinateSystemOnPlane(normals_->points[index],
                                                      u, v);

        if (boundary_estimator.isBoundaryPoint(input, index, nn_indices, u, v,
                                               float(M_PI) / 2.0))
            edge_points[index] = true;
    }

    return (edge_points);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::getScatterMatrix(
    const int &current_index, Eigen::Matrix3d &cov_m) {
    const PointInT &current_point = (*input_).points[current_index];

    double central_point[3];
    memset(central_point, 0, sizeof(double) * 3);

    central_point[0] = current_point.x;
    central_point[1] = current_point.y;
    central_point[2] = current_point.z;

    cov_m = Eigen::Matrix3d::Zero();

    std::vector<int> nn_indices;
    std::vector<float> nn_distances;
    int n_neighbors;

    this->searchForNeighbors(current_index, salient_radius_, nn_indices,
                             nn_distances);

    n_neighbors = nn_indices.size();

    if (n_neighbors < min_neighbors_)
        return;

    double cov[9];
    memset(cov, 0, sizeof(double) * 9);

    for (size_t n_idx = 0; n_idx < n_neighbors; n_idx++) {
        const PointInT &n_point = (*input_).points[nn_indices[n_idx]];

        double neigh_point[3];
        memset(neigh_point, 0, sizeof(double) * 3);

        neigh_point[0] = n_point.x;
        neigh_point[1] = n_point.y;
        neigh_point[2] = n_point.z;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                cov[i * 3 + j] += (neigh_point[i] - central_point[i]) *
                                  (neigh_point[j] - central_point[j]);
    }

    cov_m << cov[0], cov[1], cov[2], cov[3], cov[4], cov[5], cov[6], cov[7],
        cov[8];
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
bool pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::initCompute() {
    if (!Keypoint<PointInT, PointOutT>::initCompute()) {
        PCL_ERROR("[pcl::%s::initCompute] init failed!\n", name_.c_str());
        return (false);
    }
    if (salient_radius_ <= 0) {
        PCL_ERROR("[pcl::%s::initCompute] : the salient radius (%f) must be "
                  "strict positive!\n",
                  name_.c_str(), salient_radius_);
        return (false);
    }
    if (non_max_radius_ <= 0) {
        PCL_ERROR("[pcl::%s::initCompute] : the non maxima radius (%f) must be "
                  "strict positive!\n",
                  name_.c_str(), non_max_radius_);
        return (false);
    }
    if (gamma_21_ <= 0) {
        PCL_ERROR(
            "[pcl::%s::initCompute] : the threshold on the ratio between the "
            "2nd and the 1rst eigenvalue (%f) must be strict positive!\n",
            name_.c_str(), gamma_21_);
        return (false);
    }
    if (gamma_32_ <= 0) {
        PCL_ERROR(
            "[pcl::%s::initCompute] : the threshold on the ratio between the "
            "3rd and the 2nd eigenvalue (%f) must be strict positive!\n",
            name_.c_str(), gamma_32_);
        return (false);
    }
    if (min_neighbors_ <= 0) {
        PCL_ERROR("[pcl::%s::initCompute] : the minimum number of neighbors "
                  "(%f) must be strict positive!\n",
                  name_.c_str(), min_neighbors_);
        return (false);
    }
    if (normal_radius_ < 0) {
        PCL_ERROR("[pcl::%s::initCompute] : the radius used to estimate "
                  "surface normals (%f) must be positive!\n",
                  name_.c_str(), normal_radius_);
        return (false);
    }

    if (third_eigen_value_)
        delete[] third_eigen_value_;

    third_eigen_value_ = new double[input_->size()];
    memset(third_eigen_value_, 0, sizeof(double) * input_->size());

    if (edge_points_)
        delete[] edge_points_;

    if (border_radius_ > 0.0) {
        if (normal_radius_ <= 0.0) {
            PCL_ERROR("[pcl::%s::initCompute] : the normal radius (%f) must be "
                      "strict positive!\n",
                      name_.c_str(), normal_radius_);
            return (false);
        } else {
            if (normals_->empty()) {
                PointCloudNPtr normals(new PointCloudN());
                normals->reserve(surface_->size());

                if (input_->height == 1) {
                    pcl::NormalEstimation<PointInT, NormalT> normal_estimation;
                    normal_estimation.setInputCloud(surface_);
                    normal_estimation.setRadiusSearch(normal_radius_);
                    normal_estimation.compute(*normals);
                } else {
                    pcl::IntegralImageNormalEstimation<PointInT, NormalT>
                        normal_estimation;
                    normal_estimation.setNormalEstimationMethod(
                        pcl::IntegralImageNormalEstimation<
                            PointInT, NormalT>::SIMPLE_3D_GRADIENT);
                    normal_estimation.setInputCloud(surface_);
                    normal_estimation.setNormalSmoothingSize(5.0);
                    normal_estimation.compute(*normals);
                }

                normals_ = normals;
            }
        }
    } else if (border_radius_ < 0.0) {
        PCL_ERROR("[pcl::%s::initCompute] : the border radius used to estimate "
                  "boundary points (%f) must be positive!\n",
                  name_.c_str(), border_radius_);
        return (false);
    }

    return (true);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointInT, typename PointOutT, typename NormalT>
void pcl::ISSKeypoint3D<PointInT, PointOutT, NormalT>::detectKeypoints(
    PointCloudOut &output) {
    // Make sure the output cloud is empty
    output.points.clear();

    if (border_radius_ > 0.0)
        edge_points_ =
            getBoundaryPoints(*(input_->makeShared()), border_radius_);

    bool *borders = new bool[input_->size()];

    size_t index;
#ifdef _OPENMP
#pragma omp parallel for num_threads(threads_)
#endif
    for (index = 0; index < input_->size(); index++) {
        borders[index] = false;

        if (border_radius_ > 0.0) {
            std::vector<int> nn_indices;
            std::vector<float> nn_distances;

            this->searchForNeighbors(index, border_radius_, nn_indices,
                                     nn_distances);

            for (size_t j = 0; j < nn_indices.size(); j++) {
                if (edge_points_[nn_indices[j]]) {
                    borders[index] = true;
                    break;
                }
            }
        }
    }

    Eigen::Vector3d *omp_mem = new Eigen::Vector3d[threads_];

    for (int i = 0; i < threads_; i++)
        omp_mem[i].setZero(3);

    double *prg_local_mem = new double[input_->size() * 3];
    double **prg_mem = new double *[input_->size()];

    for (int i = 0; i < input_->size(); i++)
        prg_mem[i] = prg_local_mem + 3 * i;

#ifdef _OPENMP
#pragma omp parallel for num_threads(threads_)
#endif
    for (index = 0; index < input_->size(); index++) {
#ifdef _OPENMP
        int tid = omp_get_thread_num();
#else
        int tid = 0;
#endif

        if (borders[index])
            continue;

        // if the considered point is not a border point then compute the
        // scatter matrix
        Eigen::Matrix3d cov_m = Eigen::Matrix3d::Zero();

        getScatterMatrix(index, cov_m);

        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(cov_m);

        const double &e1c = solver.eigenvalues()[2];
        const double &e2c = solver.eigenvalues()[1];
        const double &e3c = solver.eigenvalues()[0];

        if (!pcl_isfinite(e1c) || !pcl_isfinite(e2c) || !pcl_isfinite(e3c))
            continue;

        omp_mem[tid][0] = e2c / e1c;
        omp_mem[tid][1] = e3c / e2c;
        ;
        omp_mem[tid][2] = e3c;

        for (int d = 0; d < omp_mem[tid].size(); ++d) {
            prg_mem[index][d] = omp_mem[tid][d];
        }
    }

    for (index = 0; index < input_->size(); index++) {
        if (borders[index])
            continue;

        if ((prg_mem[index][0] < gamma_21_) &&
            (prg_mem[index][1] < gamma_32_)) {
            if (prg_mem[index][2] < 0.0) {
                PCL_ERROR("[pcl::%s::detectKeypoints] : compute failed! The "
                          "third eigenvalue (%f) must be strict positive.\n",
                          name_.c_str(), prg_mem[index][2]);
                return;
            }

            third_eigen_value_[index] = prg_mem[index][2];
        }
    }

    bool *feat_max = new bool[input_->size()];
    bool is_max;

#ifdef _OPENMP
#pragma omp parallel for private(is_max) num_threads(threads_)
#endif
    for (index = 0; index < input_->size(); index++) {
        feat_max[index] = false;

        if (third_eigen_value_[index] > 0.0) {
            std::vector<int> nn_indices;
            std::vector<float> nn_distances;
            int n_neighbors;

            this->searchForNeighbors(index, non_max_radius_, nn_indices,
                                     nn_distances);

            n_neighbors = nn_indices.size();

            if (n_neighbors < min_neighbors_)
                continue;

            is_max = true;

            for (size_t j = 0; j < n_neighbors; j++)
                if (third_eigen_value_[index] <
                    third_eigen_value_[nn_indices[j]])
                    is_max = false;
            if (is_max)
                feat_max[index] = true;
        }
    }

#ifdef _OPENMP
#pragma omp parallel for shared(output) num_threads(threads_)
#endif
    for (index = 0; index < input_->size(); index++) {
        if (feat_max[index])
#ifdef _OPENMP
#pragma omp critical
#endif
            output.points.push_back(input_->points[index]);
    }

    output.header = input_->header;
    output.width = static_cast<uint32_t>(output.points.size());
    output.height = 1;

    // Clear the contents of variables and arrays before the beginning of the
    // next computation.
    if (border_radius_ > 0.0)
        normals_.reset(new pcl::PointCloud<NormalT>);

    delete[] borders;
    delete[] prg_mem;
    delete[] prg_local_mem;
    delete[] feat_max;
}

#define PCL_INSTANTIATE_ISSKeypoint3D(T, U, N)                                 \
    template class PCL_EXPORTS pcl::ISSKeypoint3D<T, U, N>;

#endif /* PCL_ISS_3D_IMPL_H_ */
