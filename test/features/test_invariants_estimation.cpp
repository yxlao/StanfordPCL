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
 * $Id: test_invariants_estimation.cpp 6579 2012-07-27 18:57:32Z rusu $
 *
 */

#include <gtest/gtest.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/moment_invariants.h>

using namespace pcl;
using namespace pcl::io;
using namespace std;

typedef search::KdTree<PointXYZ>::Ptr KdTreePtr;

PointCloud<PointXYZ> cloud;
vector<int> indices;
KdTreePtr tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, MomentInvariantsEstimation) {
    float j1, j2, j3;

    MomentInvariantsEstimation<PointXYZ, MomentInvariants> mi;

    // computePointMomentInvariants (indices))
    mi.computePointMomentInvariants(cloud, indices, j1, j2, j3);
    EXPECT_NEAR(j1, 1.59244, 1e-4);
    EXPECT_NEAR(j2, 0.652063, 1e-4);
    EXPECT_NEAR(j3, 0.053917, 1e-4);

    // computePointMomentInvariants
    mi.computePointMomentInvariants(cloud, indices, j1, j2, j3);
    EXPECT_NEAR(j1, 1.59244, 1e-4);
    EXPECT_NEAR(j2, 0.652063, 1e-4);
    EXPECT_NEAR(j3, 0.053917, 1e-4);

    // Object
    PointCloud<MomentInvariants>::Ptr moments(
        new PointCloud<MomentInvariants>());

    // set parameters
    mi.setInputCloud(cloud.makeShared());
    boost::shared_ptr<vector<int>> indicesptr(new vector<int>(indices));
    mi.setIndices(indicesptr);
    mi.setSearchMethod(tree);
    mi.setKSearch(static_cast<int>(indices.size()));

    // estimate
    mi.compute(*moments);
    EXPECT_EQ(moments->points.size(), indices.size());

    for (size_t i = 0; i < moments->points.size(); ++i) {
        EXPECT_NEAR(moments->points[i].j1, 1.59244, 1e-4);
        EXPECT_NEAR(moments->points[i].j2, 0.652063, 1e-4);
        EXPECT_NEAR(moments->points[i].j3, 0.053917, 1e-4);
    }
}

#ifndef PCL_ONLY_CORE_POINT_TYPES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, MomentInvariantsEstimationEigen) {
    float j1, j2, j3;

    MomentInvariantsEstimation<PointXYZ, Eigen::MatrixXf> mi;

    // computePointMomentInvariants (indices))
    mi.computePointMomentInvariants(cloud, indices, j1, j2, j3);
    EXPECT_NEAR(j1, 1.59244, 1e-4);
    EXPECT_NEAR(j2, 0.652063, 1e-4);
    EXPECT_NEAR(j3, 0.053917, 1e-4);

    // computePointMomentInvariants
    mi.computePointMomentInvariants(cloud, indices, j1, j2, j3);
    EXPECT_NEAR(j1, 1.59244, 1e-4);
    EXPECT_NEAR(j2, 0.652063, 1e-4);
    EXPECT_NEAR(j3, 0.053917, 1e-4);

    // Object
    PointCloud<Eigen::MatrixXf>::Ptr moments(new PointCloud<Eigen::MatrixXf>);

    // set parameters
    mi.setInputCloud(cloud.makeShared());
    boost::shared_ptr<vector<int>> indicesptr(new vector<int>(indices));
    mi.setIndices(indicesptr);
    mi.setSearchMethod(tree);
    mi.setKSearch(static_cast<int>(indices.size()));

    // estimate
    mi.computeEigen(*moments);
    EXPECT_EQ(moments->points.rows(), indices.size());

    for (int i = 0; i < moments->points.rows(); ++i) {
        EXPECT_NEAR(moments->points(i, 0), 1.59244, 1e-4);
        EXPECT_NEAR(moments->points(i, 1), 0.652063, 1e-4);
        EXPECT_NEAR(moments->points(i, 2), 0.053917, 1e-4);
    }
}
#endif

/* ---[ */
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "No test file given. Please download `bun0.pcd` and pass "
                     "its path to the test."
                  << std::endl;
        return (-1);
    }

    if (loadPCDFile<PointXYZ>(argv[1], cloud) < 0) {
        std::cerr << "Failed to read test file. Please download `bun0.pcd` and "
                     "pass its path to the test."
                  << std::endl;
        return (-1);
    }

    indices.resize(cloud.points.size());
    for (int i = 0; i < static_cast<int>(indices.size()); ++i)
        indices[i] = i;

    tree.reset(new search::KdTree<PointXYZ>(false));
    tree->setInputCloud(cloud.makeShared());

    testing::InitGoogleTest(&argc, argv);
    return (RUN_ALL_TESTS());
}
/* ]--- */
