/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
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

#ifndef PCL_ANGLES_H_
#define PCL_ANGLES_H_

/**
 * \file pcl/common/angles.h
 * Define standard C methods to do angle calculations
 * \ingroup common
 */

/*@{*/
namespace pcl {
/** \brief Convert an angle from radians to degrees
 * \param alpha the input angle (in radians)
 * \ingroup common
 */
inline float rad2deg(float alpha);

/** \brief Convert an angle from degrees to radians
 * \param alpha the input angle (in degrees)
 * \ingroup common
 */
inline float deg2rad(float alpha);

/** \brief Convert an angle from radians to degrees
 * \param alpha the input angle (in radians)
 * \ingroup common
 */
inline double rad2deg(double alpha);

/** \brief Convert an angle from degrees to radians
 * \param alpha the input angle (in degrees)
 * \ingroup common
 */
inline double deg2rad(double alpha);

/** \brief Normalize an angle to (-PI, PI]
 * \param alpha the input angle (in radians)
 * \ingroup common
 */
inline float normAngle(float alpha);
} // namespace pcl
/*@}*/
#include <pcl/common/impl/angles.hpp>

#endif //#ifndef PCL_NORMS_H_
