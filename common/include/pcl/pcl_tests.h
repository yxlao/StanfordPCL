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
 * $Id: pcl_tests.h 6715 2012-08-06 18:15:54Z rusu $
 */

#ifndef PCL_TEST_MACROS
#define PCL_TEST_MACROS

namespace pcl {
/** test_macros.h provide helper macros for testing vectors, matrices etc.
 * We took some liberty with upcasing names to make them look like googletest
 * macros names so that reader is not confused.
 *
 * \author Nizar Sallem
 */
namespace test {
template <typename V1, typename V2>
void EXPECT_EQ_VECTORS(const V1 &v1, const V2 &v2) {
    SCOPED_TRACE("EXPECT_EQ_VECTORS failed");
    EXPECT_EQ(v1.size(), v2.size());
    size_t length = v1.size();
    for (size_t i = 0; i < length; ++i)
        EXPECT_EQ(v1[i], v2[i]);
}

template <typename V1, typename V2, typename Scalar>
void EXPECT_NEAR_VECTORS(const V1 &v1, const V2 &v2, const Scalar &epsilon) {
    SCOPED_TRACE("EXPECT_NEAR_VECTORS failed");
    EXPECT_EQ(v1.size(), v2.size());
    size_t length = v1.size();
    for (size_t i = 0; i < length; ++i)
        EXPECT_NEAR(v1[i], v2[i], epsilon);
}
} // namespace test
} // namespace pcl

#endif
