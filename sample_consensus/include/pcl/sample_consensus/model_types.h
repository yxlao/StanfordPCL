/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2009, Willow Garage, Inc.
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
 * $Id: model_types.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_SAMPLE_CONSENSUS_MODEL_TYPES_H_
#define PCL_SAMPLE_CONSENSUS_MODEL_TYPES_H_

#include <map>

namespace pcl {
enum SacModel {
    SACMODEL_PLANE,
    SACMODEL_LINE,
    SACMODEL_CIRCLE2D,
    SACMODEL_CIRCLE3D,
    SACMODEL_SPHERE,
    SACMODEL_CYLINDER,
    SACMODEL_CONE,
    SACMODEL_TORUS,
    SACMODEL_PARALLEL_LINE,
    SACMODEL_PERPENDICULAR_PLANE,
    SACMODEL_PARALLEL_LINES,
    SACMODEL_NORMAL_PLANE,
    SACMODEL_NORMAL_SPHERE,
    SACMODEL_REGISTRATION,
    SACMODEL_PARALLEL_PLANE,
    SACMODEL_NORMAL_PARALLEL_PLANE,
    SACMODEL_STICK
};
}

// Define the number of samples in SacModel needs
typedef std::map<pcl::SacModel, unsigned int>::value_type SampleSizeModel;
const static SampleSizeModel sample_size_pairs[] = {
    SampleSizeModel(pcl::SACMODEL_PLANE, 3),
    SampleSizeModel(pcl::SACMODEL_LINE, 2),
    SampleSizeModel(pcl::SACMODEL_CIRCLE2D, 3),
    // SampleSizeModel (pcl::SACMODEL_CIRCLE3D, 3),
    SampleSizeModel(pcl::SACMODEL_SPHERE, 4),
    SampleSizeModel(pcl::SACMODEL_CYLINDER, 2),
    SampleSizeModel(pcl::SACMODEL_CONE, 3),
    // SampleSizeModel (pcl::SACMODEL_TORUS, 2),
    SampleSizeModel(pcl::SACMODEL_PARALLEL_LINE, 2),
    SampleSizeModel(pcl::SACMODEL_PERPENDICULAR_PLANE, 3),
    // SampleSizeModel (pcl::PARALLEL_LINES, 2),
    SampleSizeModel(pcl::SACMODEL_NORMAL_PLANE, 3),
    SampleSizeModel(pcl::SACMODEL_NORMAL_SPHERE, 4),
    SampleSizeModel(pcl::SACMODEL_REGISTRATION, 3),
    SampleSizeModel(pcl::SACMODEL_PARALLEL_PLANE, 3),
    SampleSizeModel(pcl::SACMODEL_NORMAL_PARALLEL_PLANE, 3),
    SampleSizeModel(pcl::SACMODEL_STICK, 2)};

namespace pcl {
const static std::map<pcl::SacModel, unsigned int> SAC_SAMPLE_SIZE(
    sample_size_pairs,
    sample_size_pairs + sizeof(sample_size_pairs) / sizeof(SampleSizeModel));
}

#endif //#ifndef PCL_SAMPLE_CONSENSUS_MODEL_TYPES_H_
