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
 * $Id: test_board_estimation.cpp 7001 2012-08-29 02:37:41Z rusu $
 *
 */

#include <gtest/gtest.h>
#include <pcl/point_cloud.h>
#include <pcl/pcl_tests.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/board.h>

using namespace pcl;
using namespace pcl::test;
using namespace pcl::io;
using namespace std;

typedef search::KdTree<PointXYZ>::Ptr KdTreePtr;

PointCloud<PointXYZ> cloud;
vector<int> indices;
KdTreePtr tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, BOARDLocalReferenceFrameEstimation) {
    PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
    PointCloud<ReferenceFrame> bunny_LRF;

    boost::shared_ptr<vector<int>> indicesptr(new vector<int>(indices));

    // Compute normals
    NormalEstimation<PointXYZ, Normal> ne;

    ne.setRadiusSearch(0.01);
    ne.setViewPoint(1, 1, 10);
    ne.setInputCloud(cloud.makeShared());
    ne.setSearchMethod(tree);
    ne.setIndices(indicesptr);

    ne.compute(*normals);

    // Compute BOARD LRF
    BOARDLocalReferenceFrameEstimation<PointXYZ, Normal, ReferenceFrame>
        lrf_estimator;

    float meshRes = 0.001f;

    lrf_estimator.setFindHoles(true);
    lrf_estimator.setRadiusSearch(15 * meshRes);
    lrf_estimator.setTangentRadius(15 * meshRes);

    lrf_estimator.setInputCloud(cloud.makeShared());
    lrf_estimator.setInputNormals(normals);
    lrf_estimator.setSearchMethod(tree);
    lrf_estimator.setIndices(indicesptr);

    lrf_estimator.compute(bunny_LRF);

    // TESTS
    EXPECT_EQ(indices.size(), bunny_LRF.size());

    EXPECT_FALSE(bunny_LRF.is_dense);
    // EXPECT_EQ (numeric_limits<float>::max (), bunny_LRF.at (24).confidence);
    EXPECT_TRUE(pcl_isnan(bunny_LRF.at(24).x_axis[0]));

    // Expected Results
    // float point_15_conf = -9.06301;
    Eigen::Vector3f point_15_x(-0.784923f, 0.208529f, 0.583448f);
    Eigen::Vector3f point_15_y(0.334206f, -0.650436f, 0.682085f);
    Eigen::Vector3f point_15_z(0.52173f, 0.730376f, 0.440851f);

    // float point_45_conf = -9.55398;
    Eigen::Vector3f point_45_x(0.909111f, 0.30943f, 0.278874f);
    Eigen::Vector3f point_45_y(-0.362239f, 0.917811f, 0.162501f);
    Eigen::Vector3f point_45_z(-0.205671f, -0.248751f, 0.946479f);

    // float point_163_conf = -9.04891;
    Eigen::Vector3f point_163_x(-0.443962f, -0.890073f, -0.103285f);
    Eigen::Vector3f point_163_y(0.746929f, -0.30394f, -0.591369f);
    Eigen::Vector3f point_163_z(0.494969f, -0.339693f, 0.799759f);

    // float point_253_conf = -9.09443;
    Eigen::Vector3f point_253_x(-0.616855f, 0.757286f, -0.214495f);
    Eigen::Vector3f point_253_y(-0.661937f, -0.646584f, -0.379168f);
    Eigen::Vector3f point_253_z(-0.425827f, -0.0919098f, 0.900124f);

    // Test Results
    // EXPECT_NEAR (point_15_conf,bunny_LRF.at (15).confidence, 1E-3);
    // EXPECT_NEAR_VECTORS (point_15_x, bunny_LRF.at
    // (15).x_axis.getNormalVector3fMap (), 1E-3); EXPECT_NEAR_VECTORS
    // (point_15_y, bunny_LRF.at (15).y_axis.getNormalVector3fMap (), 1E-3);
    // EXPECT_NEAR_VECTORS (point_15_z, bunny_LRF.at
    // (15).z_axis.getNormalVector3fMap (), 1E-3);
    for (int d = 0; d < 3; ++d) {
        EXPECT_NEAR(point_15_x[d], bunny_LRF.at(15).x_axis[d], 1E-3);
        EXPECT_NEAR(point_15_y[d], bunny_LRF.at(15).y_axis[d], 1E-3);
        EXPECT_NEAR(point_15_z[d], bunny_LRF.at(15).z_axis[d], 1E-3);

        EXPECT_NEAR(point_45_x[d], bunny_LRF.at(45).x_axis[d], 1E-3);
        EXPECT_NEAR(point_45_y[d], bunny_LRF.at(45).y_axis[d], 1E-3);
        EXPECT_NEAR(point_45_z[d], bunny_LRF.at(45).z_axis[d], 1E-3);

        EXPECT_NEAR(point_163_x[d], bunny_LRF.at(163).x_axis[d], 1E-3);
        EXPECT_NEAR(point_163_y[d], bunny_LRF.at(163).y_axis[d], 1E-3);
        EXPECT_NEAR(point_163_z[d], bunny_LRF.at(163).z_axis[d], 1E-3);

        EXPECT_NEAR(point_253_x[d], bunny_LRF.at(253).x_axis[d], 1E-3);
        EXPECT_NEAR(point_253_y[d], bunny_LRF.at(253).y_axis[d], 1E-3);
        EXPECT_NEAR(point_253_z[d], bunny_LRF.at(253).z_axis[d], 1E-3);
    }
}

#ifndef PCL_ONLY_CORE_POINT_TYPES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, BOARDLocalReferenceFrameEstimationEigen) {
    PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
    PointCloud<Eigen::MatrixXf> bunny_LRF;

    boost::shared_ptr<vector<int>> indicesptr(new vector<int>(indices));

    // Compute normals
    NormalEstimation<PointXYZ, Normal> ne;

    ne.setRadiusSearch(0.01);
    ne.setViewPoint(1, 1, 10);
    ne.setInputCloud(cloud.makeShared());
    ne.setSearchMethod(tree);
    ne.setIndices(indicesptr);

    ne.compute(*normals);

    // Compute BOARD LRF
    BOARDLocalReferenceFrameEstimation<PointXYZ, Normal, Eigen::MatrixXf>
        lrf_estimator;

    float meshRes = 0.001f;

    lrf_estimator.setFindHoles(true);
    lrf_estimator.setRadiusSearch(15 * meshRes);
    lrf_estimator.setTangentRadius(15 * meshRes);

    lrf_estimator.setInputCloud(cloud.makeShared());
    lrf_estimator.setInputNormals(normals);
    lrf_estimator.setSearchMethod(tree);
    lrf_estimator.setIndices(indicesptr);

    lrf_estimator.computeEigen(bunny_LRF);

    // TESTS
    EXPECT_EQ(indices.size(), bunny_LRF.size());

    EXPECT_FALSE(bunny_LRF.is_dense);
    // EXPECT_EQ (numeric_limits<float>::max (), bunny_LRF.points (24, 9));
    EXPECT_TRUE(pcl_isnan(bunny_LRF.points(24, 0)));

    // Expected Results
    Eigen::Vector3f point_15_x(-0.784923f, 0.208529f, 0.583448f);
    Eigen::Vector3f point_15_y(0.334206f, -0.650436f, 0.682085f);
    Eigen::Vector3f point_15_z(0.52173f, 0.730376f, 0.440851f);
    // float point_15_conf = -9.06301;

    Eigen::Vector3f point_45_x(0.909111f, 0.30943f, 0.278874f);
    Eigen::Vector3f point_45_y(-0.362239f, 0.917811f, 0.162501f);
    Eigen::Vector3f point_45_z(-0.205671f, -0.248751f, 0.946479f);
    // float point_45_conf = -9.55398;

    Eigen::Vector3f point_163_x(-0.443962f, -0.890073f, -0.103285f);
    Eigen::Vector3f point_163_y(0.746929f, -0.30394f, -0.591369f);
    Eigen::Vector3f point_163_z(0.494969f, -0.339693f, 0.799759f);
    // float point_163_conf = -9.04891;

    Eigen::Vector3f point_253_x(-0.616855f, 0.757286f, -0.214495f);
    Eigen::Vector3f point_253_y(-0.661937f, -0.646584f, -0.379168f);
    Eigen::Vector3f point_253_z(-0.425827f, -0.0919098f, 0.900124f);
    // float point_253_conf = -9.09443;

    ////Test Results
    EXPECT_NEAR_VECTORS(point_15_x, bunny_LRF.points.block<1, 3>(15, 0), 1E-3);
    EXPECT_NEAR_VECTORS(point_15_y, bunny_LRF.points.block<1, 3>(15, 3), 1E-3);
    EXPECT_NEAR_VECTORS(point_15_z, bunny_LRF.points.block<1, 3>(15, 6), 1E-3);
    // EXPECT_NEAR (point_15_conf, bunny_LRF.points (15,9), 1E-3);

    EXPECT_NEAR_VECTORS(point_45_x, bunny_LRF.points.block<1, 3>(45, 0), 1E-3);
    EXPECT_NEAR_VECTORS(point_45_y, bunny_LRF.points.block<1, 3>(45, 3), 1E-3);
    EXPECT_NEAR_VECTORS(point_45_z, bunny_LRF.points.block<1, 3>(45, 6), 1E-3);
    // EXPECT_NEAR (point_45_conf, bunny_LRF.points (45, 9), 1E-3);

    EXPECT_NEAR_VECTORS(point_163_x, bunny_LRF.points.block<1, 3>(163, 0),
                        1E-3);
    EXPECT_NEAR_VECTORS(point_163_y, bunny_LRF.points.block<1, 3>(163, 3),
                        1E-3);
    EXPECT_NEAR_VECTORS(point_163_z, bunny_LRF.points.block<1, 3>(163, 6),
                        1E-3);
    // EXPECT_NEAR (point_163_conf, bunny_LRF.points (163, 9), 1E-3);

    EXPECT_NEAR_VECTORS(point_253_x, bunny_LRF.points.block<1, 3>(253, 0),
                        1E-3);
    EXPECT_NEAR_VECTORS(point_253_y, bunny_LRF.points.block<1, 3>(253, 3),
                        1E-3);
    EXPECT_NEAR_VECTORS(point_253_z, bunny_LRF.points.block<1, 3>(253, 6),
                        1E-3);
    // EXPECT_NEAR (point_253_conf, bunny_LRF.points (253, 9), 1E-3);
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
    for (size_t i = 0; i < indices.size(); ++i)
        indices[i] = static_cast<int>(i);

    tree.reset(new search::KdTree<PointXYZ>(false));
    tree->setInputCloud(cloud.makeShared());

    testing::InitGoogleTest(&argc, argv);
    return (RUN_ALL_TESTS());
}
/* ]--- */
