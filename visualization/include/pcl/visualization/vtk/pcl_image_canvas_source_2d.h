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
 * $Id: pcl_image_canvas_source_2d.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */
#ifndef PCL_VTK_IMAGE_CANVAS_SOURCE_2D_H_
#define PCL_VTK_IMAGE_CANVAS_SOURCE_2D_H_

#include <pcl/pcl_macros.h>
#include <pcl/visualization/vtk.h>

namespace pcl
{
  namespace visualization
  {
    /** \brief pclImageCanvasSource2D represents our own custom version of vtkImageCanvasSource2D, used by
      * the ImageViewer class.
      */
    class PCL_EXPORTS PCLImageCanvasSource2D : public vtkImageCanvasSource2D
    {
      public:
        static PCLImageCanvasSource2D *New ();

      inline void
      DrawImage (vtkImageData* image)
      {
        this->ImageData->DeepCopy (image);
        this->Modified();
        return;
      }
    };
  }
}

#endif      // PCL_VTK_IMAGE_CANVAS_SOURCE_2D_H_
