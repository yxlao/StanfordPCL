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
 * $Id: test_io.cpp 6579 2012-07-27 18:57:32Z rusu $
 *
 */

#include <gtest/gtest.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_traits.h>
#include <pcl/point_types.h>
#include <pcl/common/io.h>
#include <pcl/console/print.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <fstream>
#include <locale>
#include <stdexcept>

using namespace pcl;
using namespace pcl::io;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, ComplexPCDFileASCII) {
    std::ofstream fs;
    fs.open("complex_ascii.pcd");
    fs << "# .PCD v0.7 - Point Cloud Data file format\n"
          "VERSION 0.7\n"
          "FIELDS fpfh _ x y z\n"
          "SIZE 4 1 4 4 4\n"
          "TYPE F F F F F\n"
          "COUNT 33 10 1 1 1\n"
          "WIDTH 1\n"
          "HEIGHT 1\n"
          "VIEWPOINT 0 0 0 1 0 0 0\n"
          "POINTS 1\n"
          "DATA ascii\n"
          "0 0 0 0 0 100 0 0 0 0 0 0 0 0 0 0 100 0 0 0 0 0 0 0 0 0 0 100 0 0 0 "
          "0 0 1 1 1 1 1 1 1 1 1 1 -69.234001 -65.460999 19.173";
    fs.close();

    sensor_msgs::PointCloud2 blob;
    int res = loadPCDFile("complex_ascii.pcd", blob);
    EXPECT_NE(int(res), -1);
    EXPECT_EQ(blob.width, 1);
    EXPECT_EQ(blob.height, 1);
    EXPECT_EQ(blob.is_dense, true);
    EXPECT_EQ(blob.data.size(), 4 * 33 + 10 * 1 + 4 * 3);

    // Check fields
    EXPECT_EQ(blob.fields[0].name, "fpfh");
    EXPECT_EQ(blob.fields[0].offset, 0);
    EXPECT_EQ(blob.fields[0].count, 33);
    EXPECT_EQ(blob.fields[0].datatype, sensor_msgs::PointField::FLOAT32);

    EXPECT_EQ(blob.fields[1].name, "_");
    EXPECT_EQ(blob.fields[1].offset, 4 * 33);
    EXPECT_EQ(blob.fields[1].count, 10);
    EXPECT_EQ(blob.fields[1].datatype, sensor_msgs::PointField::FLOAT32);

    EXPECT_EQ(blob.fields[2].name, "x");
    EXPECT_EQ(blob.fields[2].offset, 4 * 33 + 10 * 1);
    EXPECT_EQ(blob.fields[2].count, 1);
    EXPECT_EQ(blob.fields[2].datatype, sensor_msgs::PointField::FLOAT32);

    EXPECT_EQ(blob.fields[3].name, "y");
    EXPECT_EQ(blob.fields[3].offset, 4 * 33 + 10 * 1 + 4);
    EXPECT_EQ(blob.fields[3].count, 1);
    EXPECT_EQ(blob.fields[3].datatype, sensor_msgs::PointField::FLOAT32);

    EXPECT_EQ(blob.fields[4].name, "z");
    EXPECT_EQ(blob.fields[4].offset, 4 * 33 + 10 * 1 + 4 + 4);
    EXPECT_EQ(blob.fields[4].count, 1);
    EXPECT_EQ(blob.fields[4].datatype, sensor_msgs::PointField::FLOAT32);

    int x_idx = pcl::getFieldIndex(blob, "x");
    EXPECT_EQ(x_idx, 2);
    float x, y, z;
    memcpy(&x, &blob.data[0 * blob.point_step + blob.fields[x_idx + 0].offset],
           sizeof(float));
    memcpy(&y, &blob.data[0 * blob.point_step + blob.fields[x_idx + 1].offset],
           sizeof(float));
    memcpy(&z, &blob.data[0 * blob.point_step + blob.fields[x_idx + 2].offset],
           sizeof(float));
    EXPECT_FLOAT_EQ(x, -69.234001f);
    EXPECT_FLOAT_EQ(y, -65.460999f);
    EXPECT_FLOAT_EQ(z, 19.173f);

    int fpfh_idx = pcl::getFieldIndex(blob, "fpfh");
    EXPECT_EQ(fpfh_idx, 0);
    float val[33];
    for (size_t i = 0; i < blob.fields[fpfh_idx].count; ++i)
        memcpy(&val[i],
               &blob.data[0 * blob.point_step +
                          blob.fields[fpfh_idx + 0].offset + i * sizeof(float)],
               sizeof(float));

    EXPECT_EQ(val[0], 0);
    EXPECT_EQ(val[1], 0);
    EXPECT_EQ(val[2], 0);
    EXPECT_EQ(val[3], 0);
    EXPECT_EQ(val[4], 0);
    EXPECT_EQ(val[5], 100);
    EXPECT_EQ(val[6], 0);
    EXPECT_EQ(val[7], 0);
    EXPECT_EQ(val[8], 0);
    EXPECT_EQ(val[9], 0);
    EXPECT_EQ(val[10], 0);
    EXPECT_EQ(val[11], 0);
    EXPECT_EQ(val[12], 0);
    EXPECT_EQ(val[13], 0);
    EXPECT_EQ(val[14], 0);
    EXPECT_EQ(val[15], 0);
    EXPECT_EQ(val[16], 100);
    EXPECT_EQ(val[17], 0);
    EXPECT_EQ(val[18], 0);
    EXPECT_EQ(val[19], 0);
    EXPECT_EQ(val[20], 0);
    EXPECT_EQ(val[21], 0);
    EXPECT_EQ(val[22], 0);
    EXPECT_EQ(val[23], 0);
    EXPECT_EQ(val[24], 0);
    EXPECT_EQ(val[25], 0);
    EXPECT_EQ(val[26], 0);
    EXPECT_EQ(val[27], 100);
    EXPECT_EQ(val[28], 0);
    EXPECT_EQ(val[29], 0);
    EXPECT_EQ(val[30], 0);
    EXPECT_EQ(val[31], 0);
    EXPECT_EQ(val[32], 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, AllTypesPCDFile) {
    std::ofstream fs;
    fs.open("all_types.pcd");
    fs << "# .PCD v0.7 - Point Cloud Data file format\n"
          "VERSION 0.7\n"
          "FIELDS a1 a2 a3 a4 a5 a6 a7 a8\n"
          "SIZE    1  1  2  2  4  4  4  8\n"
          "TYPE    I  U  I  U  I  U  F  F\n"
          "COUNT   1  2  1  2  1  2  1  2\n"
          "WIDTH 1\n"
          "HEIGHT 1\n"
          "VIEWPOINT 0 0 0 1 0 0 0\n"
          "POINTS 1\n"
          "DATA ascii\n"
          "-50 250 251 -250 2500 2501 -250000 250000 250001 250.05 -250.05 "
          "-251.05";
    fs.close();

    sensor_msgs::PointCloud2 blob;
    int res = loadPCDFile("all_types.pcd", blob);
    EXPECT_NE(int(res), -1);
    EXPECT_EQ(blob.width, 1);
    EXPECT_EQ(blob.height, 1);
    EXPECT_EQ(blob.data.size(),
              1 + 1 * 2 + 2 * 1 + 2 * 2 + 4 * 1 + 4 * 2 + 4 * 1 + 8 * 2);
    EXPECT_EQ(blob.is_dense, true);

    EXPECT_EQ(blob.fields.size(), 8);
    // Check fields
    EXPECT_EQ(blob.fields[0].name, "a1");
    EXPECT_EQ(blob.fields[1].name, "a2");
    EXPECT_EQ(blob.fields[2].name, "a3");
    EXPECT_EQ(blob.fields[3].name, "a4");
    EXPECT_EQ(blob.fields[4].name, "a5");
    EXPECT_EQ(blob.fields[5].name, "a6");
    EXPECT_EQ(blob.fields[6].name, "a7");
    EXPECT_EQ(blob.fields[7].name, "a8");

    EXPECT_EQ(blob.fields[0].offset, 0);
    EXPECT_EQ(blob.fields[1].offset, 1);
    EXPECT_EQ(blob.fields[2].offset, 1 + 1 * 2);
    EXPECT_EQ(blob.fields[3].offset, 1 + 1 * 2 + 2 * 1);
    EXPECT_EQ(blob.fields[4].offset, 1 + 1 * 2 + 2 * 1 + 2 * 2);
    EXPECT_EQ(blob.fields[5].offset, 1 + 1 * 2 + 2 * 1 + 2 * 2 + 4 * 1);
    EXPECT_EQ(blob.fields[6].offset, 1 + 1 * 2 + 2 * 1 + 2 * 2 + 4 * 1 + 4 * 2);
    EXPECT_EQ(blob.fields[7].offset,
              1 + 1 * 2 + 2 * 1 + 2 * 2 + 4 * 1 + 4 * 2 + 4 * 1);

    EXPECT_EQ(blob.fields[0].count, 1);
    EXPECT_EQ(blob.fields[1].count, 2);
    EXPECT_EQ(blob.fields[2].count, 1);
    EXPECT_EQ(blob.fields[3].count, 2);
    EXPECT_EQ(blob.fields[4].count, 1);
    EXPECT_EQ(blob.fields[5].count, 2);
    EXPECT_EQ(blob.fields[6].count, 1);
    EXPECT_EQ(blob.fields[7].count, 2);

    EXPECT_EQ(blob.fields[0].datatype, sensor_msgs::PointField::INT8);
    EXPECT_EQ(blob.fields[1].datatype, sensor_msgs::PointField::UINT8);
    EXPECT_EQ(blob.fields[2].datatype, sensor_msgs::PointField::INT16);
    EXPECT_EQ(blob.fields[3].datatype, sensor_msgs::PointField::UINT16);
    EXPECT_EQ(blob.fields[4].datatype, sensor_msgs::PointField::INT32);
    EXPECT_EQ(blob.fields[5].datatype, sensor_msgs::PointField::UINT32);
    EXPECT_EQ(blob.fields[6].datatype, sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(blob.fields[7].datatype, sensor_msgs::PointField::FLOAT64);

    int8_t b1;
    uint8_t b2;
    int16_t b3;
    uint16_t b4;
    int32_t b5;
    uint32_t b6;
    float b7;
    double b8;
    memcpy(&b1, &blob.data[blob.fields[0].offset], sizeof(int8_t));
    EXPECT_FLOAT_EQ(b1, -50);
    memcpy(&b2, &blob.data[blob.fields[1].offset], sizeof(uint8_t));
    EXPECT_FLOAT_EQ(b2, 250);
    memcpy(&b2, &blob.data[blob.fields[1].offset + sizeof(uint8_t)],
           sizeof(uint8_t));
    EXPECT_FLOAT_EQ(b2, 251);
    memcpy(&b3, &blob.data[blob.fields[2].offset], sizeof(int16_t));
    EXPECT_FLOAT_EQ(b3, -250);
    memcpy(&b4, &blob.data[blob.fields[3].offset], sizeof(uint16_t));
    EXPECT_FLOAT_EQ(b4, 2500);
    memcpy(&b4, &blob.data[blob.fields[3].offset + sizeof(uint16_t)],
           sizeof(uint16_t));
    EXPECT_FLOAT_EQ(b4, 2501);
    memcpy(&b5, &blob.data[blob.fields[4].offset], sizeof(int32_t));
    EXPECT_FLOAT_EQ(float(b5), float(-250000));
    memcpy(&b6, &blob.data[blob.fields[5].offset], sizeof(uint32_t));
    EXPECT_FLOAT_EQ(float(b6), float(250000));
    memcpy(&b6, &blob.data[blob.fields[5].offset + sizeof(uint32_t)],
           sizeof(uint32_t));
    EXPECT_FLOAT_EQ(float(b6), float(250001));
    memcpy(&b7, &blob.data[blob.fields[6].offset], sizeof(float));
    EXPECT_FLOAT_EQ(b7, 250.05f);
    memcpy(&b8, &blob.data[blob.fields[7].offset], sizeof(double));
    EXPECT_FLOAT_EQ(float(b8), -250.05f);
    memcpy(&b8, &blob.data[blob.fields[7].offset + sizeof(double)],
           sizeof(double));
    EXPECT_FLOAT_EQ(float(b8), -251.05f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, ConcatenatePoints) {
    pcl::PointCloud<pcl::PointXYZ> cloud_a, cloud_b, cloud_c;

    // Fill in the cloud data
    cloud_a.width = 5;
    cloud_b.width = 3;
    cloud_a.height = cloud_b.height = 1;
    cloud_a.points.resize(cloud_a.width * cloud_a.height);
    cloud_b.points.resize(cloud_b.width * cloud_b.height);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        cloud_a.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }

    for (size_t i = 0; i < cloud_b.points.size(); ++i) {
        cloud_b.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_b.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_b.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }

    // Copy the point cloud data
    cloud_c = cloud_a;
    cloud_c += cloud_b;
    EXPECT_EQ(cloud_c.points.size(),
              cloud_a.points.size() + cloud_b.points.size());
    EXPECT_EQ(cloud_c.width, cloud_a.width + cloud_b.width);
    EXPECT_EQ(int(cloud_c.height), 1);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        EXPECT_FLOAT_EQ(cloud_c.points[i].x, cloud_a.points[i].x);
        EXPECT_FLOAT_EQ(cloud_c.points[i].y, cloud_a.points[i].y);
        EXPECT_FLOAT_EQ(cloud_c.points[i].z, cloud_a.points[i].z);
    }
    for (size_t i = cloud_a.points.size(); i < cloud_c.points.size(); ++i) {
        EXPECT_FLOAT_EQ(cloud_c.points[i].x,
                        cloud_b.points[i - cloud_a.points.size()].x);
        EXPECT_FLOAT_EQ(cloud_c.points[i].y,
                        cloud_b.points[i - cloud_a.points.size()].y);
        EXPECT_FLOAT_EQ(cloud_c.points[i].z,
                        cloud_b.points[i - cloud_a.points.size()].z);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, ConcatenateFields) {
    pcl::PointCloud<pcl::PointXYZ> cloud_a;
    pcl::PointCloud<pcl::Normal> cloud_b;
    pcl::PointCloud<pcl::PointNormal> cloud_c;

    // Fill in the cloud data
    cloud_a.width = cloud_b.width = 5;
    cloud_a.height = cloud_b.height = 1;
    cloud_a.points.resize(cloud_a.width * cloud_a.height);
    cloud_b.points.resize(cloud_b.width * cloud_b.height);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        cloud_a[i].x = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a[i].y = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a[i].z = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }

    for (size_t i = 0; i < cloud_b.points.size(); ++i) {
        cloud_b[i].normal_x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_b[i].normal_y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_b[i].normal_z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }

    pcl::concatenateFields(cloud_a, cloud_b, cloud_c);
    EXPECT_EQ(cloud_c.points.size(), cloud_a.points.size());
    EXPECT_EQ(cloud_c.width, cloud_a.width);
    EXPECT_EQ(cloud_c.height, cloud_a.height);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        EXPECT_FLOAT_EQ(cloud_c.points[i].x, cloud_a.points[i].x);
        EXPECT_FLOAT_EQ(cloud_c.points[i].y, cloud_a.points[i].y);
        EXPECT_FLOAT_EQ(cloud_c.points[i].z, cloud_a.points[i].z);
        EXPECT_FLOAT_EQ(cloud_c.points[i].normal[0],
                        cloud_b.points[i].normal[0]);
        EXPECT_FLOAT_EQ(cloud_c.points[i].normal[1],
                        cloud_b.points[i].normal[1]);
        EXPECT_FLOAT_EQ(cloud_c.points[i].normal[2],
                        cloud_b.points[i].normal[2]);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, IO) {
    sensor_msgs::PointCloud2 cloud_blob;
    PointCloud<PointXYZI> cloud;

    cloud.width = 640;
    cloud.height = 480;
    cloud.points.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.points.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud[i].x = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud[i].y = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud[i].z = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].intensity = static_cast<float>(i);
    }
    PointXYZI first, last;
    first.x = cloud.points[0].x;
    first.y = cloud.points[0].y;
    first.z = cloud.points[0].z;
    first.intensity = cloud.points[0].intensity;
    last.x = cloud.points[nr_p - 1].x;
    last.y = cloud.points[nr_p - 1].y;
    last.z = cloud.points[nr_p - 1].z;
    last.intensity = cloud.points[nr_p - 1].intensity;

    // Tests for PointCloud::operator()
    EXPECT_FLOAT_EQ(first.x, cloud(0, 0).x);
    EXPECT_FLOAT_EQ(first.y, cloud(0, 0).y);
    EXPECT_FLOAT_EQ(first.z, cloud(0, 0).z);
    EXPECT_FLOAT_EQ(first.intensity, 0.0f);
    EXPECT_FLOAT_EQ(last.x, cloud(cloud.width - 1, cloud.height - 1).x);
    EXPECT_FLOAT_EQ(last.y, cloud(cloud.width - 1, cloud.height - 1).y);
    EXPECT_FLOAT_EQ(last.z, cloud(cloud.width - 1, cloud.height - 1).z);
    EXPECT_FLOAT_EQ(last.intensity, static_cast<float>(nr_p - 1));

    // Test getFieldIndex
    std::vector<sensor_msgs::PointField> fields;
    pcl::getFields(cloud, fields);
    EXPECT_EQ(fields.size(), size_t(4));
    int x_idx = pcl::getFieldIndex(cloud, "x", fields);
    EXPECT_EQ(x_idx, 0);
    EXPECT_EQ(fields[x_idx].offset, uint32_t(0));
    EXPECT_EQ(fields[x_idx].name, "x");
    EXPECT_EQ(fields[x_idx].datatype, sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(fields[x_idx].count, uint32_t(1));

    int y_idx = pcl::getFieldIndex(cloud, "y", fields);
    EXPECT_EQ(y_idx, 1);
    EXPECT_EQ(fields[y_idx].offset, uint32_t(4));
    EXPECT_EQ(fields[y_idx].name, "y");
    EXPECT_EQ(fields[y_idx].datatype, sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(fields[y_idx].count, uint32_t(1));

    int z_idx = pcl::getFieldIndex(cloud, "z", fields);
    EXPECT_EQ(z_idx, 2);
    EXPECT_EQ(fields[z_idx].offset, uint32_t(8));
    EXPECT_EQ(fields[z_idx].name, "z");
    EXPECT_EQ(fields[z_idx].datatype, sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(fields[z_idx].count, uint32_t(1));

    int intensity_idx = pcl::getFieldIndex(cloud, "intensity", fields);
    EXPECT_EQ(intensity_idx, 3);
    EXPECT_EQ(fields[intensity_idx].offset,
              uint32_t(16)); // NOTE: intensity_idx.offset should be 12, but we
                             // are padding in PointXYZ (!)
    EXPECT_EQ(fields[intensity_idx].name, "intensity");
    EXPECT_EQ(fields[intensity_idx].datatype, sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(fields[intensity_idx].count, uint32_t(1));

    // Convert from data type to blob
    toROSMsg(cloud, cloud_blob);

    // Test getFieldIndex
    x_idx = pcl::getFieldIndex(cloud_blob, "x");
    EXPECT_EQ(x_idx, 0);
    EXPECT_EQ(cloud_blob.fields[x_idx].offset, uint32_t(0));
    EXPECT_EQ(cloud_blob.fields[x_idx].name, "x");
    EXPECT_EQ(cloud_blob.fields[x_idx].datatype,
              sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(cloud_blob.fields[x_idx].count, uint32_t(1));
    y_idx = pcl::getFieldIndex(cloud_blob, "y");
    EXPECT_EQ(y_idx, 1);
    EXPECT_EQ(cloud_blob.fields[y_idx].offset, uint32_t(4));
    EXPECT_EQ(cloud_blob.fields[y_idx].name, "y");
    EXPECT_EQ(cloud_blob.fields[y_idx].datatype,
              sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(cloud_blob.fields[y_idx].count, uint32_t(1));
    z_idx = pcl::getFieldIndex(cloud_blob, "z");
    EXPECT_EQ(z_idx, 2);
    EXPECT_EQ(cloud_blob.fields[z_idx].offset, uint32_t(8));
    EXPECT_EQ(cloud_blob.fields[z_idx].name, "z");
    EXPECT_EQ(cloud_blob.fields[z_idx].datatype,
              sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(cloud_blob.fields[z_idx].count, uint32_t(1));
    intensity_idx = pcl::getFieldIndex(cloud_blob, "intensity");
    EXPECT_EQ(intensity_idx, 3);
    // EXPECT_EQ (cloud_blob.fields[intensity_idx].offset, (uint32_t)12); //
    // NOTE: the fields.offset is 16 in PointCloud<PointXYZI>, but we are
    // obtaining the correct offset in toROSMsg
    EXPECT_EQ(
        cloud_blob.fields[intensity_idx].offset,
        uint32_t(
            16)); // NOTE: the fields.offset is 16 in PointCloud<PointXYZI>, but
                  // we are obtaining the correct offset in toROSMsg
    EXPECT_EQ(cloud_blob.fields[intensity_idx].name, "intensity");
    EXPECT_EQ(cloud_blob.fields[intensity_idx].datatype,
              sensor_msgs::PointField::FLOAT32);
    EXPECT_EQ(cloud_blob.fields[intensity_idx].count, uint32_t(1));

    fromROSMsg(cloud_blob, cloud);
    for (size_t i = 0; i < nr_p; ++i)
        EXPECT_EQ(cloud.points[i].intensity, i);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width); // test for toROSMsg ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for toROSMsg ()
    EXPECT_EQ(bool(cloud_blob.is_dense),
              cloud.is_dense); // test for toROSMsg ()
    // EXPECT_EQ ((size_t)cloud_blob.data.size () * 2,         // PointXYZI is
    // 16*2 (XYZ+1, Intensity+3)
    //           cloud_blob.width * cloud_blob.height * sizeof (PointXYZI));  //
    //           test for toROSMsg ()
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height *
            sizeof(PointXYZI)); // test for toROSMsg ()

    // Make sure we have permissions to write there
    PCDWriter w;
    int res =
        w.writeASCII("test_pcl_io.pcd", cloud_blob, Eigen::Vector4f::Zero(),
                     Eigen::Quaternionf::Identity(), 10);
    EXPECT_EQ(int(res), 0); // test for savePCDFileASCII ()

    // Please make sure that this file exists, otherwise the test will fail.
    res = loadPCDFile("test_pcl_io.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense),
              cloud.is_dense); // test for loadPCDFile ()
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    // Make sure we have permissions to write there
    res = savePCDFile("test_pcl_io.pcd", cloud_blob, Eigen::Vector4f::Zero(),
                      Eigen::Quaternionf::Identity(), true);
    EXPECT_EQ(int(res), 0); // test for savePCDFileBinary ()

    // Please make sure that this file exists, otherwise the test will fail.
    res = loadPCDFile("test_pcl_io.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    // Save as ASCII
    try {
        w.write<PointXYZI>("test_pcl_io_binary.pcd", cloud, true);
    } catch (pcl::IOException &e) {
        std::cerr << e.detailedMessage() << std::endl;
    }
    res = loadPCDFile("test_pcl_io_binary.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    // Save as ASCII
    try {
        w.write<PointXYZI>("test_pcl_io_ascii.pcd", cloud, false);
    } catch (pcl::IOException &e) {
        std::cerr << e.detailedMessage() << std::endl;
    }
    res = loadPCDFile("test_pcl_io_ascii.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense), true);
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(float(cloud.points[nr_p - 1].intensity),
                    float(last.intensity)); // test for fromROSMsg ()

    std::vector<int> indices(cloud.width * cloud.height / 2);
    for (int i = 0; i < static_cast<int>(indices.size()); ++i)
        indices[i] = i;
    // Save as ASCII
    try {
        w.write<PointXYZI>("test_pcl_io_binary.pcd", cloud, indices, true);
    } catch (pcl::IOException &e) {
        std::cerr << e.detailedMessage() << std::endl;
    }
    res = loadPCDFile("test_pcl_io_binary.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width * cloud.height / 2); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height), 1); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);                   // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p / 2); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(float(cloud.points[0].intensity),
                    float(first.intensity)); // test for fromROSMsg ()

    indices.resize(cloud.width * cloud.height / 2);
    for (int i = 0; i < static_cast<int>(indices.size()); ++i)
        indices[i] = i;
    // Save as ASCII
    try {
        w.write<PointXYZI>("test_pcl_io_ascii.pcd", cloud, indices, false);
    } catch (pcl::IOException &e) {
        std::cerr << e.detailedMessage() << std::endl;
    }
    res = loadPCDFile("test_pcl_io_ascii.pcd", cloud_blob);
    EXPECT_NE(int(res), -1); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.width),
              cloud.width * cloud.height / 2); // test for loadPCDFile ()
    EXPECT_EQ(uint32_t(cloud_blob.height), 1); // test for loadPCDFile ()
    EXPECT_EQ(bool(cloud_blob.is_dense), true);
    EXPECT_EQ(size_t(cloud_blob.data.size() *
                     2), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
              cloud_blob.width * cloud_blob.height *
                  sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);                   // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p / 4); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, PCDReaderWriter) {
    sensor_msgs::PointCloud2 cloud_blob;
    PointCloud<PointXYZI> cloud;

    cloud.width = 640;
    cloud.height = 480;
    cloud.points.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.points.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].intensity = static_cast<float>(i);
    }
    PointXYZI first, last;
    first.x = cloud.points[0].x;
    first.y = cloud.points[0].y;
    first.z = cloud.points[0].z;
    first.intensity = cloud.points[0].intensity;
    last.x = cloud.points[nr_p - 1].x;
    last.y = cloud.points[nr_p - 1].y;
    last.z = cloud.points[nr_p - 1].z;
    last.intensity = cloud.points[nr_p - 1].intensity;

    // Convert from data type to blob
    toROSMsg(cloud, cloud_blob);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width); // test for toROSMsg ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for toROSMsg ()
    EXPECT_EQ(bool(cloud_blob.is_dense),
              cloud.is_dense); // test for toROSMsg ()
    // EXPECT_EQ ((size_t)cloud_blob.data.size () * 2,         // PointXYZI is
    // 16*2 (XYZ+1, Intensity+3)
    //           cloud_blob.width * cloud_blob.height * sizeof (PointXYZI));  //
    //           test for toROSMsg ()
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height *
            sizeof(PointXYZI)); // test for toROSMsg ()

    PCDWriter writer;
    writer.write("test_pcl_io.pcd", cloud_blob, Eigen::Vector4f::Zero(),
                 Eigen::Quaternionf::Identity(), true);

    PCDReader reader;
    reader.read("test_pcl_io.pcd", cloud_blob);
    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_blob.height), cloud.height);
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    // EXPECT_EQ ((size_t)cloud_blob.data.size () * 2,         // PointXYZI is
    // 16*2 (XYZ+1, Intensity+3)
    //           cloud_blob.width * cloud_blob.height * sizeof (PointXYZI));  //
    //           test for loadPCDFile ()
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height *
            sizeof(PointXYZI)); // test for loadPCDFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, PCDReaderWriterEigen) {
    sensor_msgs::PointCloud2 cloud_blob;
    PointCloud<PointXYZI> cloud;
    pcl::PointCloud<Eigen::MatrixXf> cloud_eigen1, cloud_eigen2, cloud_eigen3;

    cloud.width = 640;
    cloud.height = 480;
    cloud.points.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.points.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].intensity = static_cast<float>(i);
    }
    PointXYZI first, last;
    first.x = cloud.points[0].x;
    first.y = cloud.points[0].y;
    first.z = cloud.points[0].z;
    first.intensity = cloud.points[0].intensity;
    last.x = cloud.points[nr_p - 1].x;
    last.y = cloud.points[nr_p - 1].y;
    last.z = cloud.points[nr_p - 1].z;
    last.intensity = cloud.points[nr_p - 1].intensity;

    // Convert from data type to blob
    toROSMsg(cloud, cloud_blob);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width); // test for toROSMsg ()
    EXPECT_EQ(uint32_t(cloud_blob.height),
              cloud.height); // test for toROSMsg ()
    EXPECT_EQ(bool(cloud_blob.is_dense),
              cloud.is_dense); // test for toROSMsg ()
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height * sizeof(PointXYZI));

    // Convert to Eigen
    cloud_eigen1 = cloud;

    PCDWriter writer;
    writer.writeBinaryCompressedEigen("test_pcl_io.pcd", cloud_eigen1);

    PCDReader reader;
    reader.read("test_pcl_io.pcd", cloud_blob);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_blob.height), cloud.height);
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height * sizeof(PointXYZI) /
            2); // we already got rid of the padding here

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    cloud_eigen2 = cloud;
    writer.writeBinaryEigen("test_pcl_io.pcd", cloud_eigen2);

    reader.read("test_pcl_io.pcd", cloud_blob);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_blob.height), cloud.height);
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height * sizeof(PointXYZI) /
            2); // we already got rid of the padding here

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    cloud_eigen3 = cloud;
    writer.writeASCIIEigen("test_pcl_io.pcd", cloud_eigen3);

    reader.read("test_pcl_io.pcd", cloud_blob);

    EXPECT_EQ(uint32_t(cloud_blob.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_blob.height), cloud.height);
    EXPECT_EQ(bool(cloud_blob.is_dense), cloud.is_dense);
    EXPECT_EQ(
        size_t(
            cloud_blob.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob.width * cloud_blob.height * sizeof(PointXYZI) /
            2); // we already got rid of the padding here

    // Convert from blob to data type
    fromROSMsg(cloud_blob, cloud);

    EXPECT_EQ(uint32_t(cloud.width),
              cloud_blob.width); // test for fromROSMsg ()
    EXPECT_EQ(uint32_t(cloud.height),
              cloud_blob.height); // test for fromROSMsg ()
    EXPECT_EQ(int(cloud.is_dense),
              cloud_blob.is_dense);               // test for fromROSMsg ()
    EXPECT_EQ(size_t(cloud.points.size()), nr_p); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[0].x, first.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].y, first.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].z, first.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[0].intensity,
                    first.intensity); // test for fromROSMsg ()

    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].x, last.x); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].y, last.y); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].z, last.z); // test for fromROSMsg ()
    EXPECT_FLOAT_EQ(cloud.points[nr_p - 1].intensity,
                    last.intensity); // test for fromROSMsg ()

    pcl::PointCloud<Eigen::MatrixXf> cloud_eigen4, cloud_eigen5, cloud_eigen6;
    reader.readEigen("test_pcl_io.pcd", cloud_eigen4);

    EXPECT_EQ(uint32_t(cloud_eigen4.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_eigen4.height), cloud.height);
    EXPECT_EQ(bool(cloud_eigen4.is_dense), cloud.is_dense);

    for (int i = 0; i < cloud_eigen4.points.rows(); ++i) {
        EXPECT_FLOAT_EQ(cloud_eigen4.points(i, 0), cloud.points[i].x);
        EXPECT_FLOAT_EQ(cloud_eigen4.points(i, 1), cloud.points[i].y);
        EXPECT_FLOAT_EQ(cloud_eigen4.points(i, 2), cloud.points[i].z);
        EXPECT_FLOAT_EQ(cloud_eigen4.points(i, 3), cloud.points[i].intensity);
    }

    writer.writeBinaryEigen("test_pcl_io.pcd", cloud_eigen1);

    reader.readEigen("test_pcl_io.pcd", cloud_eigen5);

    EXPECT_EQ(uint32_t(cloud_eigen5.width), cloud.width);
    EXPECT_EQ(uint32_t(cloud_eigen5.height), cloud.height);
    EXPECT_EQ(bool(cloud_eigen5.is_dense), cloud.is_dense);

    for (int i = 0; i < cloud_eigen5.points.rows(); ++i) {
        EXPECT_FLOAT_EQ(cloud_eigen5.points(i, 0), cloud.points[i].x);
        EXPECT_FLOAT_EQ(cloud_eigen5.points(i, 1), cloud.points[i].y);
        EXPECT_FLOAT_EQ(cloud_eigen5.points(i, 2), cloud.points[i].z);
        EXPECT_FLOAT_EQ(cloud_eigen5.points(i, 3), cloud.points[i].intensity);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, PLYReaderWriter) {
    sensor_msgs::PointCloud2 cloud_blob, cloud_blob2;
    PointCloud<PointXYZI> cloud, cloud2;

    cloud.width = 640;
    cloud.height = 480;
    cloud.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud[i].x = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud[i].y = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud[i].z = static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud[i].intensity = static_cast<float>(i);
    }

    // Convert from data type to blob
    toROSMsg(cloud, cloud_blob);

    EXPECT_EQ(cloud_blob.width, cloud.width);       // test for toROSMsg ()
    EXPECT_EQ(cloud_blob.height, cloud.height);     // test for toROSMsg ()
    EXPECT_EQ(cloud_blob.is_dense, cloud.is_dense); // test for toROSMsg ()
    EXPECT_EQ(cloud_blob.data.size(),
              cloud_blob.width * cloud_blob.height * sizeof(PointXYZI));

    // test for toROSMsg ()
    PLYWriter writer;
    writer.write("test_pcl_io.ply", cloud_blob, Eigen::Vector4f::Zero(),
                 Eigen::Quaternionf::Identity(), true, true);

    PLYReader reader;
    reader.read("test_pcl_io.ply", cloud_blob2);
    // PLY DOES preserve organiziation
    EXPECT_EQ(cloud_blob.width * cloud_blob.height,
              cloud_blob2.width * cloud_blob2.height);
    EXPECT_EQ(cloud_blob.is_dense, cloud.is_dense);
    EXPECT_EQ(
        size_t(
            cloud_blob2.data.size()), // PointXYZI is 16*2 (XYZ+1, Intensity+3)
        cloud_blob2.width * cloud_blob2.height *
            sizeof(PointXYZ)); // test for loadPLYFile ()

    // Convert from blob to data type
    fromROSMsg(cloud_blob2, cloud2);

    // EXPECT_EQ (cloud.width, cloud2.width);    // test for fromROSMsg ()
    // EXPECT_EQ (cloud.height, cloud2.height);  // test for fromROSMsg ()
    // EXPECT_EQ (cloud.is_dense, cloud2.is_dense);   // test for fromROSMsg ()
    EXPECT_EQ(cloud.size(), cloud2.size()); // test for fromROSMsg ()

    for (uint32_t counter = 0; counter < cloud.size(); ++counter) {
        EXPECT_FLOAT_EQ(cloud[counter].x,
                        cloud2[counter].x); // test for fromROSMsg ()
        EXPECT_FLOAT_EQ(cloud[counter].y,
                        cloud2[counter].y); // test for fromROSMsg ()
        EXPECT_FLOAT_EQ(cloud[counter].z,
                        cloud2[counter].z); // test for fromROSMsg ()
        EXPECT_FLOAT_EQ(cloud[counter].intensity,
                        cloud2[counter].intensity); // test for fromROSMsg ()
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PointXYZFPFH33 {
    float x, y, z;
    float histogram[33];
};
POINT_CLOUD_REGISTER_POINT_STRUCT(
    PointXYZFPFH33,
    (float, x, x)(float, y, y)(float, z, z)(float[33], histogram, fpfh))

inline std::ostream &operator<<(std::ostream &os, const PointXYZFPFH33 &p) {
    os << "(" << p.x << "," << p.y << "," << p.z << ")";
    for (int i = 0; i < 33; ++i)
        os << (i == 0 ? "(" : "") << p.histogram[i] << (i < 32 ? ", " : ")");
    return os;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, ExtendedIO) {
    PointCloud<PointXYZFPFH33> cloud;
    cloud.width = 2;
    cloud.height = 1;
    cloud.points.resize(2);

    cloud.points[0].x = cloud.points[0].y = cloud.points[0].z = 1;
    cloud.points[1].x = cloud.points[1].y = cloud.points[1].z = 2;
    for (int i = 0; i < 33; ++i) {
        cloud.points[0].histogram[i] = static_cast<float>(i);
        cloud.points[1].histogram[i] = 33.0f - static_cast<float>(i);
    }

    savePCDFile("v.pcd", cloud);
    cloud.points.clear();
    loadPCDFile("v.pcd", cloud);

    EXPECT_EQ(int(cloud.width), 2);
    EXPECT_EQ(int(cloud.height), 1);
    EXPECT_EQ(cloud.is_dense, true);
    EXPECT_EQ(int(cloud.points.size()), 2);

    EXPECT_EQ(cloud.points[0].x, 1);
    EXPECT_EQ(cloud.points[0].y, 1);
    EXPECT_EQ(cloud.points[0].z, 1);
    EXPECT_EQ(cloud.points[1].x, 2);
    EXPECT_EQ(cloud.points[1].y, 2);
    EXPECT_EQ(cloud.points[1].z, 2);
    for (int i = 0; i < 33; ++i) {
        ASSERT_EQ(cloud.points[0].histogram[i], i);
        ASSERT_EQ(cloud.points[1].histogram[i], 33 - i);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, EigenConversions) {
    PointCloud<PointXYZ> cloud;
    cloud.points.resize(5);

    for (int i = 0; i < int(cloud.points.size()); ++i)
        cloud.points[i].x = cloud.points[i].y = cloud.points[i].z =
            static_cast<float>(i);

    sensor_msgs::PointCloud2 blob;
    toROSMsg(cloud, blob);

    Eigen::MatrixXf mat;
    getPointCloudAsEigen(blob, mat);
    EXPECT_EQ(mat.cols(), int(cloud.points.size()));
    EXPECT_EQ(mat.rows(), 4);

    for (size_t i = 0; i < cloud.points.size(); ++i) {
        EXPECT_EQ(mat(0, i), cloud.points[i].x);
        EXPECT_EQ(mat(1, i), cloud.points[i].y);
        EXPECT_EQ(mat(2, i), cloud.points[i].z);
        EXPECT_EQ(mat(3, i), 1);
    }

    getEigenAsPointCloud(mat, blob);
    fromROSMsg(blob, cloud);
    for (size_t i = 0; i < cloud.points.size(); ++i) {
        EXPECT_EQ(cloud.points[i].x, i);
        EXPECT_EQ(cloud.points[i].y, i);
        EXPECT_EQ(cloud.points[i].z, i);
    }

    getPointCloudAsEigen(blob, mat);
    EXPECT_EQ(mat.cols(), int(cloud.points.size()));
    EXPECT_EQ(mat.rows(), 4);

    for (size_t i = 0; i < cloud.points.size(); ++i) {
        EXPECT_EQ(mat(0, i), cloud.points[i].x);
        EXPECT_EQ(mat(1, i), cloud.points[i].y);
        EXPECT_EQ(mat(2, i), cloud.points[i].z);
        EXPECT_EQ(mat(3, i), 1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, CopyPointCloud) {
    pcl::PointCloud<pcl::PointXYZ> cloud_a;
    pcl::PointCloud<pcl::PointXYZRGBA> cloud_b;

    // Fill in the cloud data
    cloud_a.width = cloud_b.width = 3;
    cloud_a.height = cloud_b.height = 1;
    cloud_a.points.resize(cloud_a.width * cloud_a.height);
    cloud_b.points.resize(cloud_b.width * cloud_b.height);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        cloud_a.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_a.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud_b.points[i].rgba = 255;
    }

    pcl::copyPointCloud<pcl::PointXYZ, pcl::PointXYZRGBA>(cloud_a, cloud_b);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        EXPECT_EQ(cloud_b.points[i].x, cloud_a.points[i].x);
        EXPECT_EQ(cloud_b.points[i].y, cloud_a.points[i].y);
        EXPECT_EQ(cloud_b.points[i].z, cloud_a.points[i].z);
        EXPECT_EQ(cloud_b.points[i].rgba, 255);
        cloud_a.points[i].x = cloud_a.points[i].y = cloud_a.points[i].z = 0;
    }

    pcl::copyPointCloud<pcl::PointXYZRGBA, pcl::PointXYZ>(cloud_b, cloud_a);

    for (size_t i = 0; i < cloud_a.points.size(); ++i) {
        EXPECT_EQ(cloud_b.points[i].x, cloud_a.points[i].x);
        EXPECT_EQ(cloud_b.points[i].y, cloud_a.points[i].y);
        EXPECT_EQ(cloud_b.points[i].z, cloud_a.points[i].z);
        EXPECT_EQ(cloud_b.points[i].rgba, 255);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, LZF) {
    PointCloud<PointXYZ> cloud, cloud2;
    cloud.width = 640;
    cloud.height = 480;
    cloud.points.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.points.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }
    PCDWriter writer;
    int res = writer.writeBinaryCompressed<PointXYZ>(
        "test_pcl_io_compressed.pcd", cloud);
    EXPECT_EQ(res, 0);

    PCDReader reader;
    reader.read<PointXYZ>("test_pcl_io_compressed.pcd", cloud2);

    EXPECT_EQ(cloud2.width, cloud.width);
    EXPECT_EQ(cloud2.height, cloud.height);
    EXPECT_EQ(cloud2.is_dense, cloud.is_dense);
    EXPECT_EQ(cloud2.points.size(), cloud.points.size());

    for (size_t i = 0; i < cloud2.points.size(); ++i) {
        ASSERT_EQ(cloud2.points[i].x, cloud.points[i].x);
        ASSERT_EQ(cloud2.points[i].y, cloud.points[i].y);
        ASSERT_EQ(cloud2.points[i].z, cloud.points[i].z);
    }

    sensor_msgs::PointCloud2 blob;
    pcl::toROSMsg(cloud, blob);
    res = writer.writeBinaryCompressed("test_pcl_io_compressed.pcd", blob);
    EXPECT_EQ(res, 0);

    reader.read<PointXYZ>("test_pcl_io_compressed.pcd", cloud2);

    EXPECT_EQ(cloud2.width, blob.width);
    EXPECT_EQ(cloud2.height, blob.height);
    EXPECT_EQ(cloud2.is_dense, cloud.is_dense);
    EXPECT_EQ(cloud2.points.size(), cloud.points.size());

    for (size_t i = 0; i < cloud2.points.size(); ++i) {
        EXPECT_EQ(cloud2.points[i].x, cloud.points[i].x);
        EXPECT_EQ(cloud2.points[i].y, cloud.points[i].y);
        EXPECT_EQ(cloud2.points[i].z, cloud.points[i].z);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, LZFExtended) {
    PointCloud<PointXYZRGBNormal> cloud, cloud2;
    cloud.width = 640;
    cloud.height = 480;
    cloud.points.resize(cloud.width * cloud.height);
    cloud.is_dense = true;

    srand(static_cast<unsigned int>(time(NULL)));
    size_t nr_p = cloud.points.size();
    // Randomly create a new point cloud
    for (size_t i = 0; i < nr_p; ++i) {
        cloud.points[i].x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].normal_x =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].normal_y =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].normal_z =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        cloud.points[i].rgb =
            static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
    }

    sensor_msgs::PointCloud2 blob;
    pcl::toROSMsg(cloud, blob);

    PCDWriter writer;
    int res = writer.writeBinaryCompressed("test_pcl_io_compressed.pcd", blob);
    EXPECT_EQ(res, 0);

    PCDReader reader;
    reader.read<PointXYZRGBNormal>("test_pcl_io_compressed.pcd", cloud2);

    EXPECT_EQ(cloud2.width, blob.width);
    EXPECT_EQ(cloud2.height, blob.height);
    EXPECT_EQ(cloud2.is_dense, cloud.is_dense);
    EXPECT_EQ(cloud2.points.size(), cloud.points.size());

    for (size_t i = 0; i < cloud2.points.size(); ++i) {
        EXPECT_EQ(cloud2.points[i].x, cloud.points[i].x);
        EXPECT_EQ(cloud2.points[i].y, cloud.points[i].y);
        EXPECT_EQ(cloud2.points[i].z, cloud.points[i].z);
        EXPECT_EQ(cloud2.points[i].normal_x, cloud.points[i].normal_x);
        EXPECT_EQ(cloud2.points[i].normal_y, cloud.points[i].normal_y);
        EXPECT_EQ(cloud2.points[i].normal_z, cloud.points[i].normal_z);
        EXPECT_EQ(cloud2.points[i].rgb, cloud.points[i].rgb);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TEST(PCL, Locale) {
#ifndef __APPLE__
    try {
        PointCloud<PointXYZ> cloud, cloud2;
        cloud.width = 640;
        cloud.height = 480;
        cloud.points.resize(cloud.width * cloud.height);
        cloud.is_dense = true;

        srand(static_cast<unsigned int>(time(NULL)));
        size_t nr_p = cloud.points.size();
        // Randomly create a new point cloud
        cloud.points[0].x = std::numeric_limits<float>::quiet_NaN();
        cloud.points[0].y = std::numeric_limits<float>::quiet_NaN();
        cloud.points[0].z = std::numeric_limits<float>::quiet_NaN();

        for (size_t i = 1; i < nr_p; ++i) {
            cloud.points[i].x =
                static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
            cloud.points[i].y =
                static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
            cloud.points[i].z =
                static_cast<float>(1024 * rand() / (RAND_MAX + 1.0));
        }
        PCDWriter writer;
        try {
#ifdef _WIN32
            std::locale::global(std::locale("German_germany"));
#else
            std::locale::global(std::locale("de_DE.UTF-8"));
#endif
        } catch (std::runtime_error e) {
            PCL_WARN("Failed to set locale, skipping test.\n");
        }
        int res = writer.writeASCII<PointXYZ>("test_pcl_io_ascii.pcd", cloud);
        EXPECT_EQ(res, 0);

        PCDReader reader;
        try {
#ifdef _WIN32
            std::locale::global(std::locale("English_US"));
#else
            std::locale::global(std::locale("en_US.UTF-8"));
#endif
        } catch (std::runtime_error e) {
            PCL_WARN("Failed to set locale, skipping test.\n");
        }
        reader.read<PointXYZ>("test_pcl_io_ascii.pcd", cloud2);
        std::locale::global(std::locale::classic());

        EXPECT_EQ(cloud2.width, cloud.width);
        EXPECT_EQ(cloud2.height, cloud.height);
        EXPECT_EQ(cloud2.is_dense, false);
        EXPECT_EQ(cloud2.points.size(), cloud.points.size());

        EXPECT_TRUE(pcl_isnan(cloud2.points[0].x));
        EXPECT_TRUE(pcl_isnan(cloud2.points[0].y));
        EXPECT_TRUE(pcl_isnan(cloud2.points[0].z));
        for (size_t i = 1; i < cloud2.points.size(); ++i) {
            ASSERT_FLOAT_EQ(cloud2.points[i].x, cloud.points[i].x);
            ASSERT_FLOAT_EQ(cloud2.points[i].y, cloud.points[i].y);
            ASSERT_FLOAT_EQ(cloud2.points[i].z, cloud.points[i].z);
        }
    } catch (std::exception &e) {
    }
#endif
}

/* ---[ */
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return (RUN_ALL_TESTS());
}
/* ]--- */
