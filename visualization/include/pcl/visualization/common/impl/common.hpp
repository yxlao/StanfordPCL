/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
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
 * $Id: common.hpp 6883 2012-08-20 19:53:35Z svn $
 *
 */

namespace pcl {
namespace visualization {
/** \brief Converts point to window coordinates
 * \param[in] pt xyz point to be converted
 * \param[out] window_cord vector containing the pts' window X,Y, Z and 1
 * \note This function computes the projection and view matrix every time.
 * It is very inefficient to use this for every point in the point cloud!
 */
template <typename PointT>
void Camera::cvtWindowCoordinates(const PointT &pt,
                                  Eigen::Vector4d &window_cord) const {
    Eigen::Matrix4d proj, view;
    this->computeViewMatrix(view);
    this->computeProjectionMatrix(proj);
    this->cvtWindowCoordinates(pt, window_cord, proj * view);
    return;
}

/** \brief converts point to window coordiantes
 * \param[in] pt xyz point to be converted
 * \param[out] window_cord vector containing the pts' window X,Y, Z and 1
 * \param[in] composite_mat composite transformation matrix (proj*view)
 *
 * \note Use this function to compute window coordinates with a pre-computed
 * transformation function. The typical composite matrix will be the projection
 * matrix * the view matrix. However, additional matrices like a camera
 * distortion matrix can also be added.
 */
template <typename PointT>
void Camera::cvtWindowCoordinates(const PointT &pt,
                                  Eigen::Vector4d &window_cord,
                                  const Eigen::Matrix4d &composite_mat) const {
    Eigen::Vector4d pte(pt.x, pt.y, pt.z, 1);
    window_cord = composite_mat * pte;
    window_cord = window_cord / window_cord(3);
    window_cord[0] = (window_cord[0] + 1.0) / 2.0 * window_size[0];
    window_cord[1] = (window_cord[1] + 1.0) / 2.0 * window_size[1];
    window_cord[2] = (window_cord[2] + 1.0) / 2.0;
}
} // namespace visualization
} // namespace pcl
