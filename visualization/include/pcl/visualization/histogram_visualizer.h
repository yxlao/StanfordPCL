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
 * $Id: histogram_visualizer.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */
#ifndef PCL_PCL_HISTOGRAM_VISUALIZER_H_
#define PCL_PCL_HISTOGRAM_VISUALIZER_H_

#include <pcl/visualization/interactor_style.h>
#include <pcl/visualization/vtk.h>
#include <pcl/visualization/common/common.h>
#include <pcl/visualization/common/ren_win_interact_map.h>

namespace pcl {
namespace visualization {
/** \brief PCL histogram visualizer main class.
 * \author Radu Bogdan Rusu
 * \ingroup visualization
 */
class PCL_EXPORTS PCLHistogramVisualizer {
  public:
    /** \brief PCL histogram visualizer constructor. */
    PCLHistogramVisualizer();

    virtual ~PCLHistogramVisualizer() {}
    /** \brief Spin once method. Calls the interactor and updates the screen
     * once. \param[in] time - How long (in ms) should the visualization loop be
     * allowed to run.
     */
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION <= 4))
    void spinOnce(int time = 1, bool force_redraw = false);
#else
    void spinOnce(int time = 1);
#endif
    /** \brief Spin method. Calls the interactor and runs an internal loop. */
    void spin();

    /** \brief Set the viewport's background color.
     * \param[in] r the red component of the RGB color
     * \param[in] g the green component of the RGB color
     * \param[in] b the blue component of the RGB color
     * \param[in] viewport the view port (default: all)
     */
    void setBackgroundColor(const double &r, const double &g, const double &b,
                            int viewport = 0);

    /** \brief Add a histogram feature to screen as a separate window, from a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] hsize the length of the
     * histogram \param[in] id the point cloud object id (default: cloud)
     * \param[in] win_width the width of the window
     * \param[in] win_height the height of the window
     */
    template <typename PointT>
    bool addFeatureHistogram(const pcl::PointCloud<PointT> &cloud, int hsize,
                             const std::string &id = "cloud",
                             int win_width = 640, int win_height = 200);

    /** \brief Add a histogram feature to screen as a separate window from a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] field_name the field name
     * containing the histogram \param[in] id the point cloud object id
     * (default: cloud) \param[in] win_width the width of the window \param[in]
     * win_height the height of the window
     */
    bool addFeatureHistogram(const sensor_msgs::PointCloud2 &cloud,
                             const std::string &field_name,
                             const std::string &id = "cloud",
                             int win_width = 640, int win_height = 200);

    /** \brief Add a histogram feature to screen as a separate window.
     * \param[in] cloud the PointCloud dataset containing the histogram
     * \param[in] field_name the field name containing the histogram
     * \param[in] index the point index to extract the histogram from
     * \param[in] id the point cloud object id (default: cloud)
     * \param[in] win_width the width of the window
     * \param[in] win_height the height of the window
     */
    template <typename PointT>
    bool addFeatureHistogram(const pcl::PointCloud<PointT> &cloud,
                             const std::string &field_name, const int index,
                             const std::string &id = "cloud",
                             int win_width = 640, int win_height = 200);

    /** \brief Add a histogram feature to screen as a separate window.
     * \param[in] cloud the PointCloud dataset containing the histogram
     * \param[in] field_name the field name containing the histogram
     * \param[in] index the point index to extract the histogram from
     * \param[in] id the point cloud object id (default: cloud)
     * \param[in] win_width the width of the window
     * \param[in] win_height the height of the window
     */
    bool addFeatureHistogram(const sensor_msgs::PointCloud2 &cloud,
                             const std::string &field_name, const int index,
                             const std::string &id = "cloud",
                             int win_width = 640, int win_height = 200);

    /** \brief Update a histogram feature that is already on screen, with a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] hsize the length of the
     * histogram \param[in] id the point cloud object id (default: cloud)
     */
    template <typename PointT>
    bool updateFeatureHistogram(const pcl::PointCloud<PointT> &cloud, int hsize,
                                const std::string &id = "cloud");

    /** \brief Update a histogram feature that is already on screen, with a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] field_name the field name
     * containing the histogram \param[in] id the point cloud object id
     * (default: cloud)
     */
    bool updateFeatureHistogram(const sensor_msgs::PointCloud2 &cloud,
                                const std::string &field_name,
                                const std::string &id = "cloud");

    /** \brief Update a histogram feature that is already on screen, with a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] field_name the field name
     * containing the histogram \param[in] index the point index to extract the
     * histogram from \param[in] id the point cloud object id (default: cloud)
     */
    template <typename PointT>
    bool updateFeatureHistogram(const pcl::PointCloud<PointT> &cloud,
                                const std::string &field_name, const int index,
                                const std::string &id = "cloud");

    /** \brief Update a histogram feature that is already on screen, with a
     * cloud containing a single histogram. \param[in] cloud the PointCloud
     * dataset containing the histogram \param[in] field_name the field name
     * containing the histogram \param[in] index the point index to extract the
     * histogram from \param[in] id the point cloud object id (default: cloud)
     */
    bool updateFeatureHistogram(const sensor_msgs::PointCloud2 &cloud,
                                const std::string &field_name, const int index,
                                const std::string &id = "cloud");

    /** \brief Set the Y range to minp-maxp for all histograms.
     * \param[in] minp the minimum Y range
     * \param[in] maxp the maximum Y range
     */
    void setGlobalYRange(float minp, float maxp);

    /** \brief Update all window positions on screen so that they fit. */
    void updateWindowPositions();
#if ((VTK_MAJOR_VERSION) == 5 && (VTK_MINOR_VERSION <= 4))
    /** \brief Returns true when the user tried to close the window */
    bool wasStopped();

    /** \brief Set the stopped flag back to false */
    void resetStoppedFlag();
#endif
  protected:
    /** \brief Create a 2D actor from the given vtkDoubleArray histogram and add
     * it to the screen. \param[in] xy_array the input vtkDoubleArray holding
     * the histogram data \param[out] renwinint the resultant render window
     * interactor holding the rendered object \param[in] id the point cloud
     * object id \param[in] win_width the width of the window \param[in]
     * win_height the height of the window
     */
    void createActor(const vtkSmartPointer<vtkDoubleArray> &xy_array,
                     RenWinInteract &renwinint, const std::string &id,
                     const int win_width, const int win_height);

    /** \brief Remove the current 2d actor and create a new 2D actor from the
     * given vtkDoubleArray histogram and add it to the screen. \param[in]
     * xy_array the input vtkDoubleArray holding the histogram data \param[out]
     * renwinint the resultant render window interactor holding the rendered
     * object \param[in] id the point cloud object id
     */
    void reCreateActor(const vtkSmartPointer<vtkDoubleArray> &xy_array,
                       RenWinInteract *renwinupd, const int hsize);

  private:
    /** \brief A map of all windows on screen (with their renderers and
     * interactors). */
    RenWinInteractMap wins_;

    struct ExitMainLoopTimerCallback : public vtkCommand {
        static ExitMainLoopTimerCallback *New() {
            return (new ExitMainLoopTimerCallback);
        }
        virtual void Execute(vtkObject *vtkNotUsed(caller),
                             unsigned long event_id, void *call_data) {
            if (event_id != vtkCommand::TimerEvent)
                return;
            int timer_id = *(reinterpret_cast<int *>(call_data));

            if (timer_id != right_timer_id)
                return;

                // Stop vtk loop and send notification to app to wake it up
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION <= 4))
            interact->stopLoop();
#else
            interact->TerminateApp();
#endif
        }
        int right_timer_id;
#if ((VTK_MAJOR_VERSION == 5) && (VTK_MINOR_VERSION <= 4))
        PCLVisualizerInteractor *interact;
#else
        vtkRenderWindowInteractor *interact;
#endif
    };

    struct ExitCallback : public vtkCommand {
        ExitCallback() : his() {}

        static ExitCallback *New() { return (new ExitCallback); }

        virtual void Execute(vtkObject *, unsigned long event_id, void *) {
            if (event_id != vtkCommand::ExitEvent)
                return;
            his->stopped_ = true;
        }
        PCLHistogramVisualizer *his;
    };

    /** \brief Callback object enabling us to leave the main loop, when a timer
     * fires. */
    vtkSmartPointer<ExitMainLoopTimerCallback> exit_main_loop_timer_callback_;
    vtkSmartPointer<ExitCallback> exit_callback_;
    /** \brief Set to true when the histogram visualizer is ready to be
     * terminated. */
    bool stopped_;
};
} // namespace visualization
} // namespace pcl

#include <pcl/visualization/impl/histogram_visualizer.hpp>

#endif
