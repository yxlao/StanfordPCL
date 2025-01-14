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
 */

#ifndef PCL_ML_MULTI_CHANNEL_2D_COMPARISON_FEATURE_HANDLER_H_
#define PCL_ML_MULTI_CHANNEL_2D_COMPARISON_FEATURE_HANDLER_H_

#include <pcl/common/common.h>

#include <pcl/ml/feature_handler.h>
#include <pcl/ml/multi_channel_2d_data_set.h>
#include <pcl/ml/multi_channel_2d_comparison_feature.h>
#include <pcl/ml/multiple_data_2d_example_index.h>
#include <pcl/ml/point_xy_32i.h>
#include <pcl/ml/point_xy_32f.h>

#include <istream>
#include <ostream>

namespace pcl {

/** \brief Feature utility class that handles the creation and evaluation of
 * RGBD comparison features. */
template <class DATA_TYPE, size_t NUM_OF_CHANNELS>
class PCL_EXPORTS MultiChannel2DComparisonFeatureHandler
    : public pcl::FeatureHandler<
          pcl::MultiChannel2DComparisonFeature<pcl::PointXY32i>,
          pcl::MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS>,
          pcl::MultipleData2DExampleIndex> {

  public:
    /** \brief Constructor. */
    MultiChannel2DComparisonFeatureHandler(const int feature_window_width,
                                           const int feature_window_height)
        : feature_window_width_(feature_window_width),
          feature_window_height_(feature_window_height) {}
    /** \brief Destructor. */
    virtual ~MultiChannel2DComparisonFeatureHandler() {}

    /** \brief Sets the feature window size.
     * \param[in] width The width of the feature window.
     * \param[in] height The height of the feature window.
     */
    inline void setFeatureWindowSize(int width, int height) {
        feature_window_width_ = width;
        feature_window_height_ = height;
    }

    /** \brief Creates random features.
     * \param[in] num_of_features The number of random features to create.
     * \param[out] features The destination for the created random features.
     */
    inline void createRandomFeatures(
        const size_t num_of_features,
        std::vector<MultiChannel2DComparisonFeature<PointXY32i>> &features) {
        features.resize(num_of_features);
        for (size_t feature_index = 0; feature_index < num_of_features;
             ++feature_index) {
            features[feature_index].p1 = PointXY32i::randomPoint(
                -feature_window_width_ / 2, feature_window_width_ / 2,
                -feature_window_height_ / 2, feature_window_height_ / 2);
            features[feature_index].p2 = PointXY32i::randomPoint(
                -feature_window_width_ / 2, feature_window_width_ / 2,
                -feature_window_height_ / 2, feature_window_height_ / 2);
            features[feature_index].channel = static_cast<unsigned char>(
                NUM_OF_CHANNELS *
                (static_cast<float>(rand()) / (RAND_MAX + 1)));
        }
    }

    /** \brief Evaluates a feature for a set of examples on the specified data
     * set. \param[in] feature The feature to evaluate. \param[in] data_set The
     * data set the feature is evaluated on. \param[in] examples The examples
     * the feature is evaluated for. \param[out] results The destination for the
     * evaluation results. \param[out] flags The destination for the flags
     * corresponding to the evaluation results.
     */
    inline void
    evaluateFeature(const MultiChannel2DComparisonFeature<PointXY32i> &feature,
                    MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS> &data_set,
                    std::vector<MultipleData2DExampleIndex> &examples,
                    std::vector<float> &results,
                    std::vector<unsigned char> &flags) const {
        results.resize(examples.size());
        flags.resize(examples.size());
        for (int example_index = 0; example_index < examples.size();
             ++example_index) {
            const MultipleData2DExampleIndex &example = examples[example_index];

            evaluateFeature(feature, data_set, example, results[example_index],
                            flags[example_index]);
        }
    }

    /** \brief Evaluates a feature for one examples on the specified data set.
     * \param[in] feature The feature to evaluate.
     * \param[in] data_set The data set the feature is evaluated on.
     * \param[in] example The example the feature is evaluated for.
     * \param[out] result The destination for the evaluation result.
     * \param[out] flag The destination for the flag corresponding to the
     * evaluation result.
     */
    inline void
    evaluateFeature(const MultiChannel2DComparisonFeature<PointXY32i> &feature,
                    MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS> &data_set,
                    const MultipleData2DExampleIndex &example, float &result,
                    unsigned char &flag) const {
        const int center_col_index = example.x;
        const int center_row_index = example.y;

        const size_t p1_col =
            static_cast<size_t>(feature.p1.x + center_col_index);
        const size_t p1_row =
            static_cast<size_t>(feature.p1.y + center_row_index);

        const size_t p2_col =
            static_cast<size_t>(feature.p2.x + center_col_index);
        const size_t p2_row =
            static_cast<size_t>(feature.p2.y + center_row_index);

        const unsigned char channel = feature.channel;

        const float value1 = static_cast<float>(
            data_set(example.data_set_id, p1_col, p1_row)[channel]);
        const float value2 = static_cast<float>(
            data_set(example.data_set_id, p2_col, p2_row)[channel]);

        result = value1 - value2;
        flag = (pcl_isfinite(value1) && pcl_isfinite(value2)) ? 0 : 1;
    }

    /** \brief Generates code for feature evaluation.
     * \param[in] feature The feature for which code is generated.
     * \param[out] stream The destination for the generated code.
     */
    void generateCodeForEvaluation(
        const MultiChannel2DComparisonFeature<PointXY32i> &feature,
        std::ostream &stream) const {
        stream << "ERROR: RegressionVarianceStatsEstimator does not implement "
                  "generateCodeForBranchIndex(...)";
        // stream << "const float value = ( (*dataSet)(dataSetId, centerY+" <<
        // feature.p1.y << ", centerX+" << feature.p1.x << ")[" <<
        // static_cast<int>(feature.colorChannel) << "]"
        //  << " - " << "(*dataSet)(dataSetId, centerY+" << feature.p2.y << ",
        //  centerX+" << feature.p2.x << ")[" <<
        //  static_cast<int>(feature.colorChannel) << "] );" << ::std::endl;
    }

  private:
    /** \brief The width of the feature window. */
    int feature_window_width_;
    /** \brief The height of the feature window. */
    int feature_window_height_;
};

/** \brief Feature utility class that handles the creation and evaluation of
 * RGBD comparison features. */
template <class DATA_TYPE, size_t NUM_OF_CHANNELS, size_t SCALE_CHANNEL>
class PCL_EXPORTS ScaledMultiChannel2DComparisonFeatureHandler
    : public pcl::FeatureHandler<
          pcl::MultiChannel2DComparisonFeature<pcl::PointXY32f>,
          pcl::MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS>,
          pcl::MultipleData2DExampleIndex> {

  public:
    /** \brief Constructor. */
    ScaledMultiChannel2DComparisonFeatureHandler(
        const int feature_window_width, const int feature_window_height)
        : feature_window_width_(feature_window_width),
          feature_window_height_(feature_window_height) {}
    /** \brief Destructor. */
    virtual ~ScaledMultiChannel2DComparisonFeatureHandler() {}

    /** \brief Sets the feature window size.
     * \param[in] width The width of the feature window.
     * \param[in] height The height of the feature window.
     */
    inline void setFeatureWindowSize(int width, int height) {
        feature_window_width_ = width;
        feature_window_height_ = height;
    }

    /** \brief Creates random features.
     * \param[in] num_of_features The number of random features to create.
     * \param[out] features The destination for the created random features.
     */
    inline void createRandomFeatures(
        const size_t num_of_features,
        std::vector<MultiChannel2DComparisonFeature<PointXY32f>> &features) {
        features.resize(num_of_features);
        for (size_t feature_index = 0; feature_index < num_of_features;
             ++feature_index) {
            features[feature_index].p1 = PointXY32f::randomPoint(
                -feature_window_width_ / 2, feature_window_width_ / 2,
                -feature_window_height_ / 2, feature_window_height_ / 2);
            features[feature_index].p2 = PointXY32f::randomPoint(
                -feature_window_width_ / 2, feature_window_width_ / 2,
                -feature_window_height_ / 2, feature_window_height_ / 2);
            features[feature_index].channel = static_cast<unsigned char>(
                NUM_OF_CHANNELS *
                (static_cast<float>(rand()) / (RAND_MAX + 1)));
        }
    }

    /** \brief Evaluates a feature for a set of examples on the specified data
     * set. \param[in] feature The feature to evaluate. \param[in] data_set The
     * data set the feature is evaluated on. \param[in] examples The examples
     * the feature is evaluated for. \param[out] results The destination for the
     * evaluation results. \param[out] flags The destination for the flags
     * corresponding to the evaluation results.
     */
    inline void
    evaluateFeature(const MultiChannel2DComparisonFeature<PointXY32f> &feature,
                    MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS> &data_set,
                    std::vector<MultipleData2DExampleIndex> &examples,
                    std::vector<float> &results,
                    std::vector<unsigned char> &flags) const {
        results.resize(examples.size());
        flags.resize(examples.size());
        for (int example_index = 0; example_index < examples.size();
             ++example_index) {
            const MultipleData2DExampleIndex &example = examples[example_index];

            evaluateFeature(feature, data_set, example, results[example_index],
                            flags[example_index]);
        }
    }

    /** \brief Evaluates a feature for one examples on the specified data set.
     * \param[in] feature The feature to evaluate.
     * \param[in] data_set The data set the feature is evaluated on.
     * \param[in] example The example the feature is evaluated for.
     * \param[out] result The destination for the evaluation result.
     * \param[out] flag The destination for the flag corresponding to the
     * evaluation result.
     */
    inline void
    evaluateFeature(const MultiChannel2DComparisonFeature<PointXY32f> &feature,
                    MultiChannel2DDataSet<DATA_TYPE, NUM_OF_CHANNELS> &data_set,
                    const MultipleData2DExampleIndex &example, float &result,
                    unsigned char &flag) const {
        const int center_col_index = example.x;
        const int center_row_index = example.y;

        const float scale =
            static_cast<float>(data_set(example.data_set_id, center_col_index,
                                        center_row_index)[SCALE_CHANNEL]);

        const size_t p1_col =
            static_cast<size_t>(scale * feature.p1.x + center_col_index);
        const size_t p1_row =
            static_cast<size_t>(scale * feature.p1.y + center_row_index);

        const size_t p2_col =
            static_cast<size_t>(scale * feature.p2.x + center_col_index);
        const size_t p2_row =
            static_cast<size_t>(scale * feature.p2.y + center_row_index);

        const unsigned char channel = feature.channel;

        const float value1 = static_cast<float>(
            data_set(example.data_set_id, p1_col, p1_row)[channel]);
        const float value2 = static_cast<float>(
            data_set(example.data_set_id, p2_col, p2_row)[channel]);

        result = value1 - value2;
        flag = (pcl_isfinite(value1) && pcl_isfinite(value2)) ? 0 : 1;
    }

    /** \brief Generates code for feature evaluation.
     * \param[in] feature The feature for which code is generated.
     * \param[out] stream The destination for the generated code.
     */
    void generateCodeForEvaluation(
        const MultiChannel2DComparisonFeature<PointXY32f> &feature,
        std::ostream &stream) const {
        stream << "ERROR: RegressionVarianceStatsEstimator does not implement "
                  "generateCodeForBranchIndex(...)";
        // stream << "const float value = ( (*dataSet)(dataSetId, centerY+" <<
        // feature.p1.y << ", centerX+" << feature.p1.x << ")[" <<
        // static_cast<int>(feature.colorChannel) << "]"
        //  << " - " << "(*dataSet)(dataSetId, centerY+" << feature.p2.y << ",
        //  centerX+" << feature.p2.x << ")[" <<
        //  static_cast<int>(feature.colorChannel) << "] );" << ::std::endl;
    }

  private:
    /** \brief The width of the feature window. */
    int feature_window_width_;
    /** \brief The height of the feature window. */
    int feature_window_height_;
};

typedef MultiChannel2DComparisonFeatureHandler<float, 1>
    Depth2DComparisonFeatureHandler;
typedef MultiChannel2DComparisonFeatureHandler<float, 2>
    IntensityDepth2DComparisonFeatureHandler;
typedef MultiChannel2DComparisonFeatureHandler<float, 3>
    RGB2DComparisonFeatureHandler;
typedef MultiChannel2DComparisonFeatureHandler<float, 4>
    RGBD2DComparisonFeatureHandler;

typedef ScaledMultiChannel2DComparisonFeatureHandler<float, 1, 0>
    ScaledDepth2DComparisonFeatureHandler;
typedef ScaledMultiChannel2DComparisonFeatureHandler<float, 2, 1>
    ScaledIntensityDepth2DComparisonFeatureHandler;
typedef ScaledMultiChannel2DComparisonFeatureHandler<float, 4, 3>
    ScaledRGBD2DComparisonFeatureHandler;

} // namespace pcl

#endif
