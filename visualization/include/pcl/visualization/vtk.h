/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2009-2012, Willow Garage, Inc.
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
 * $Id: vtk.h 6949 2012-08-24 21:25:25Z rusu $
 *
 */
#ifndef PCL_PCL_VISUALIZER_VTK_H_
#define PCL_PCL_VISUALIZER_VTK_H_

#if defined __GNUC__
#pragma GCC system_header
#ifdef __DEPRECATED
#undef __DEPRECATED
#define __DEPRECATED_DISABLED__
#endif
#endif

#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkAssemblyPath.h>
#include <vtkAxes.h>
#include <vtkAxesActor.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget.h>
#include <vtkBoxWidget2.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkClipPolyData.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDiskSource.h>
#include <vtkExecutive.h>
#include <vtkFloatArray.h>
#include <vtkFollower.h>
#include <vtkGeneralTransform.h>
#include <vtkIdentityTransform.h>
#include <vtkImageImport.h>
#include <vtkImageViewer.h>
#include <vtkInformationVector.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLegendScaleActor.h>
#include <vtkLightKit.h>
#include <vtkLineSource.h>
#include <vtkLoopSubdivisionFilter.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkPLYReader.h>
#include <vtkPNGWriter.h>
#include <vtkPlaneSource.h>
#include <vtkPlanes.h>
#include <vtkPlatonicSolidSource.h>
#include <vtkPointData.h>
#include <vtkPointPicker.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyLine.h>
#include <vtkPolygon.h>
#include <vtkPropPicker.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTubeFilter.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVectorText.h>
#include <vtkWindowToImageFilter.h>
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION >= 10))
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#endif
#include <vtkBMPWriter.h>
#include <vtkCleanPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkFieldData.h>
#include <vtkImageBlend.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageFlip.h>
#include <vtkImageReslice.h>
#include <vtkImageStencilData.h>
#include <vtkImageViewer2.h>
#include <vtkJPEGWriter.h>
#include <vtkLODActor.h>
#include <vtkObject.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTIFFWriter.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkXYPlotActor.h>
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION <= 4))
#include <pcl/visualization/interactor.h>
#else
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColorSeries.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkPlot.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTable.h>
#endif
#include <vtkSelection.h>

#if VTK_MAJOR_VERSION == 6 || (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION > 4)
#include <vtkHardwareSelector.h>
#else
#include <vtkVisibleCellSelector.h>
#endif

#include <vtkTriangle.h>
#include <vtkWorldPointPicker.h>

#include <vtkAreaPicker.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkIdFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkVertexGlyphFilter.h>

#if defined __GNUC__ && defined __DEPRECATED_DISABLED__
#define __DEPRECATED
#undef __DEPRECATED_DISABLED__
#endif

#endif // PCL_PCL_VISUALIZER_VTK_H_
