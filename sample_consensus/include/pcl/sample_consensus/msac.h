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
 * $Id: msac.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_SAMPLE_CONSENSUS_MSAC_H_
#define PCL_SAMPLE_CONSENSUS_MSAC_H_

#include <algorithm>
#include <pcl/sample_consensus/sac.h>
#include <pcl/sample_consensus/sac_model.h>

namespace pcl
{
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /** \brief @b MEstimatorSampleConsensus represents an implementation of the MSAC (M-estimator SAmple Consensus)
    * algorithm, as described in: "MLESAC: A new robust estimator with application to estimating image geometry", P.H.S.
    * Torr and A. Zisserman, Computer Vision and Image Understanding, vol 78, 2000.
    * \author Radu Bogdan Rusu
    * \ingroup sample_consensus
    */
  template <typename PointT>
  class MEstimatorSampleConsensus : public SampleConsensus<PointT>
  {
    using SampleConsensus<PointT>::max_iterations_;
    using SampleConsensus<PointT>::threshold_;
    using SampleConsensus<PointT>::iterations_;
    using SampleConsensus<PointT>::sac_model_;
    using SampleConsensus<PointT>::model_;
    using SampleConsensus<PointT>::model_coefficients_;
    using SampleConsensus<PointT>::inliers_;
    using SampleConsensus<PointT>::probability_;

    typedef typename SampleConsensusModel<PointT>::Ptr SampleConsensusModelPtr;

    public:
      /** \brief MSAC (M-estimator SAmple Consensus) main constructor
        * \param model a Sample Consensus model
        */
      MEstimatorSampleConsensus (const SampleConsensusModelPtr &model) : SampleConsensus<PointT> (model)
      {
        // Maximum number of trials before we give up.
        max_iterations_ = 10000;
      }

      /** \brief MSAC (M-estimator SAmple Consensus) main constructor
        * \param model a Sample Consensus model
        * \param threshold distance to model threshold
        */
      MEstimatorSampleConsensus (const SampleConsensusModelPtr &model, double threshold) : SampleConsensus<PointT> (model, threshold)
      {
        // Maximum number of trials before we give up.
        max_iterations_ = 10000;
      }

      /** \brief Compute the actual model and find the inliers
        * \param debug_verbosity_level enable/disable on-screen debug information and set the verbosity level
        */
      bool computeModel (int debug_verbosity_level = 0);
  };
}

#endif  //#ifndef PCL_SAMPLE_CONSENSUS_MSAC_H_
