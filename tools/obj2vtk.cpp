/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2012-, Open Perception, Inc.
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
 * $Id: obj2pcd.cpp 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#include <pcl/console/print.h>
#include <pcl/console/parse.h>
#include <pcl/io/vtk_lib_io.h>

using namespace pcl;
using namespace pcl::io;
using namespace pcl::console;

void printHelp(int, char **argv) {
    print_error("Syntax is: %s input.obj output.vtk\n", argv[0]);
}

/* ---[ */
int main(int argc, char **argv) {
    print_info(
        "Convert a OBJ file to VTK format. For more information, use: %s -h\n",
        argv[0]);

    if (argc < 3) {
        printHelp(argc, argv);
        return (-1);
    }

    // Parse the command line arguments for .vtk and .obj files
    std::vector<int> vtk_file_indices =
        parse_file_extension_argument(argc, argv, ".vtk");
    std::vector<int> obj_file_indices =
        parse_file_extension_argument(argc, argv, ".obj");
    if (vtk_file_indices.size() != 1 || obj_file_indices.size() != 1) {
        print_error("Need one input OBJ file and one output VTK file.\n");
        return (-1);
    }

    // Load the input file
    vtkSmartPointer<vtkPolyData> polydata;
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(argv[obj_file_indices[0]]);
    polydata = reader->GetOutput();
    polydata->Update();

    // Convert to VTK and save
    vtkSmartPointer<vtkPolyDataWriter> writer =
        vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetInput(polydata);
    writer->SetFileName(argv[vtk_file_indices[0]]);
    writer->Write();
}
