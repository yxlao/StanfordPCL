/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
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
 * $Id: add_gaussian_noise.cpp 6697 2012-08-04 02:41:10Z rusu $
 *
 */

#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/console/print.h>
#include <pcl/console/parse.h>
#include <pcl/console/time.h>
#include "boost.h"

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

double default_standard_deviation = 0.01;

void printHelp(int, char **argv) {
    print_error("Syntax is: %s input.pcd output.pcd <options>\n", argv[0]);
    print_info("  where options are:\n");
    print_info("                     -sd X = the standard deviation for the "
               "normal distribution (default: ");
    print_value("%f", default_standard_deviation);
    print_info(")\n");
}

bool loadCloud(const std::string &filename, sensor_msgs::PointCloud2 &cloud) {
    TicToc tt;
    print_highlight("Loading ");
    print_value("%s ", filename.c_str());

    tt.tic();
    if (loadPCDFile(filename, cloud) < 0)
        return (false);
    print_info("[done, ");
    print_value("%g", tt.toc());
    print_info(" ms: ");
    print_value("%d", cloud.width * cloud.height);
    print_info(" points]\n");
    print_info("Available dimensions: ");
    print_value("%s\n", pcl::getFieldsList(cloud).c_str());

    return (true);
}

void compute(const sensor_msgs::PointCloud2::ConstPtr &input,
             sensor_msgs::PointCloud2 &output, double standard_deviation) {
    TicToc tt;
    tt.tic();

    print_highlight(
        "Adding Gaussian noise with mean 0.0 and standard deviation %f\n",
        standard_deviation);

    PointCloud<PointXYZ>::Ptr xyz_cloud(new pcl::PointCloud<PointXYZ>());
    fromROSMsg(*input, *xyz_cloud);

    PointCloud<PointXYZ>::Ptr xyz_cloud_filtered(new PointCloud<PointXYZ>());
    xyz_cloud_filtered->points.resize(xyz_cloud->points.size());
    xyz_cloud_filtered->header = xyz_cloud->header;
    xyz_cloud_filtered->width = xyz_cloud->width;
    xyz_cloud_filtered->height = xyz_cloud->height;

    boost::mt19937 rng;
    rng.seed(static_cast<unsigned int>(time(0)));
    boost::normal_distribution<> nd(0, standard_deviation);
    boost::variate_generator<boost::mt19937 &, boost::normal_distribution<>>
        var_nor(rng, nd);

    for (size_t point_i = 0; point_i < xyz_cloud->points.size(); ++point_i) {
        xyz_cloud_filtered->points[point_i].x =
            xyz_cloud->points[point_i].x + static_cast<float>(var_nor());
        xyz_cloud_filtered->points[point_i].y =
            xyz_cloud->points[point_i].y + static_cast<float>(var_nor());
        xyz_cloud_filtered->points[point_i].z =
            xyz_cloud->points[point_i].z + static_cast<float>(var_nor());
    }

    sensor_msgs::PointCloud2 input_xyz_filtered;
    toROSMsg(*xyz_cloud_filtered, input_xyz_filtered);
    concatenateFields(*input, input_xyz_filtered, output);

    print_info("[done, ");
    print_value("%g", tt.toc());
    print_info(" ms: ");
    print_value("%d", output.width * output.height);
    print_info(" points]\n");
}

void saveCloud(const std::string &filename,
               const sensor_msgs::PointCloud2 &output) {
    TicToc tt;
    tt.tic();

    print_highlight("Saving ");
    print_value("%s ", filename.c_str());

    pcl::io::savePCDFile(filename, output);

    print_info("[done, ");
    print_value("%g", tt.toc());
    print_info(" ms: ");
    print_value("%d", output.width * output.height);
    print_info(" points]\n");
}

/* ---[ */
int main(int argc, char **argv) {
    print_info("Add Gaussian noise to a point cloud. For more information, "
               "use: %s -h\n",
               argv[0]);

    if (argc < 3) {
        printHelp(argc, argv);
        return (-1);
    }

    // Parse the command line arguments for .pcd files
    std::vector<int> p_file_indices;
    p_file_indices = parse_file_extension_argument(argc, argv, ".pcd");
    if (p_file_indices.size() != 2) {
        print_error(
            "Need one input PCD file and one output PCD file to continue.\n");
        return (-1);
    }

    // Command line parsing
    double standard_deviation = default_standard_deviation;
    parse_argument(argc, argv, "-sd", standard_deviation);

    // Load the first file
    sensor_msgs::PointCloud2::Ptr cloud(new sensor_msgs::PointCloud2);
    if (!loadCloud(argv[p_file_indices[0]], *cloud))
        return (-1);

    // Add the noise
    sensor_msgs::PointCloud2 output;
    compute(cloud, output, standard_deviation);

    // Save into the second file
    saveCloud(argv[p_file_indices[1]], output);
}
