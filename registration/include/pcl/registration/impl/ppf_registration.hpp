/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Alexandru-Eugen Ichim
 *                      Willow Garage, Inc
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
 * $Id: ppf_registration.hpp 7153 2012-09-16 22:24:29Z aichim $
 *
 */

#ifndef PCL_REGISTRATION_IMPL_PPF_REGISTRATION_H_
#define PCL_REGISTRATION_IMPL_PPF_REGISTRATION_H_

//#include <pcl/registration/ppf_registration.h>
#include <pcl/features/ppf.h>
#include <pcl/common/transforms.h>

#include <pcl/features/pfh.h>
//////////////////////////////////////////////////////////////////////////////////////////////
void pcl::PPFHashMapSearch::setInputFeatureCloud(
    PointCloud<PPFSignature>::ConstPtr feature_cloud) {
    // Discretize the feature cloud and insert it in the hash map
    feature_hash_map_->clear();
    unsigned int n = static_cast<unsigned int>(
        sqrt(static_cast<float>(feature_cloud->points.size())));
    int d1, d2, d3, d4;
    max_dist_ = -1.0;
    alpha_m_.resize(n);
    for (size_t i = 0; i < n; ++i) {
        std::vector<float> alpha_m_row(n);
        for (size_t j = 0; j < n; ++j) {
            d1 = static_cast<int>(floor(feature_cloud->points[i * n + j].f1 /
                                        angle_discretization_step_));
            d2 = static_cast<int>(floor(feature_cloud->points[i * n + j].f2 /
                                        angle_discretization_step_));
            d3 = static_cast<int>(floor(feature_cloud->points[i * n + j].f3 /
                                        angle_discretization_step_));
            d4 = static_cast<int>(floor(feature_cloud->points[i * n + j].f4 /
                                        distance_discretization_step_));
            feature_hash_map_->insert(
                std::pair<HashKeyStruct, std::pair<size_t, size_t>>(
                    HashKeyStruct(d1, d2, d3, d4),
                    std::pair<size_t, size_t>(i, j)));
            alpha_m_row[j] = feature_cloud->points[i * n + j].alpha_m;

            if (max_dist_ < feature_cloud->points[i * n + j].f4)
                max_dist_ = feature_cloud->points[i * n + j].f4;
        }
        alpha_m_[i] = alpha_m_row;
    }

    internals_initialized_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
void pcl::PPFHashMapSearch::nearestNeighborSearch(
    float &f1, float &f2, float &f3, float &f4,
    std::vector<std::pair<size_t, size_t>> &indices) {
    if (!internals_initialized_) {
        PCL_ERROR("[pcl::PPFRegistration::nearestNeighborSearch]: input "
                  "feature cloud has not been set - skipping search!\n");
        return;
    }

    int d1 = static_cast<int>(floor(f1 / angle_discretization_step_)),
        d2 = static_cast<int>(floor(f2 / angle_discretization_step_)),
        d3 = static_cast<int>(floor(f3 / angle_discretization_step_)),
        d4 = static_cast<int>(floor(f4 / distance_discretization_step_));

    indices.clear();
    HashKeyStruct key = HashKeyStruct(d1, d2, d3, d4);
    std::pair<FeatureHashMapType::iterator, FeatureHashMapType::iterator>
        map_iterator_pair = feature_hash_map_->equal_range(key);
    for (; map_iterator_pair.first != map_iterator_pair.second;
         ++map_iterator_pair.first)
        indices.push_back(
            std::pair<size_t, size_t>(map_iterator_pair.first->second.first,
                                      map_iterator_pair.first->second.second));
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void pcl::PPFRegistration<PointSource, PointTarget>::setInputTarget(
    const PointCloudTargetConstPtr &cloud) {
    Registration<PointSource, PointTarget>::setInputTarget(cloud);

    scene_search_tree_ = typename pcl::KdTreeFLANN<PointTarget>::Ptr(
        new pcl::KdTreeFLANN<PointTarget>);
    scene_search_tree_->setInputCloud(target_);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void pcl::PPFRegistration<PointSource, PointTarget>::computeTransformation(
    PointCloudSource &output, const Eigen::Matrix4f &guess) {
    if (!search_method_) {
        PCL_ERROR("[pcl::PPFRegistration::computeTransformation] Search method "
                  "not set - skipping computeTransformation!\n");
        return;
    }

    if (guess != Eigen::Matrix4f::Identity()) {
        PCL_ERROR("[pcl::PPFRegistration::computeTransformation] setting "
                  "initial transform (guess) not implemented!\n");
    }

    PoseWithVotesList voted_poses;
    std::vector<std::vector<unsigned int>> accumulator_array;
    accumulator_array.resize(input_->points.size());

    size_t aux_size = static_cast<size_t>(
        floor(2 * M_PI / search_method_->getAngleDiscretizationStep()));
    for (size_t i = 0; i < input_->points.size(); ++i) {
        std::vector<unsigned int> aux(aux_size);
        accumulator_array[i] = aux;
    }
    PCL_INFO("Accumulator array size: %u x %u.\n", accumulator_array.size(),
             accumulator_array.back().size());

    // Consider every <scene_reference_point_sampling_rate>-th point as the
    // reference point => fix s_r
    float f1, f2, f3, f4;
    for (size_t scene_reference_index = 0;
         scene_reference_index < target_->points.size();
         scene_reference_index += scene_reference_point_sampling_rate_) {
        Eigen::Vector3f
            scene_reference_point =
                target_->points[scene_reference_index].getVector3fMap(),
            scene_reference_normal =
                target_->points[scene_reference_index].getNormalVector3fMap();

        Eigen::AngleAxisf rotation_sg(
            acosf(scene_reference_normal.dot(Eigen::Vector3f::UnitX())),
            scene_reference_normal.cross(Eigen::Vector3f::UnitX())
                .normalized());
        Eigen::Affine3f transform_sg(
            Eigen::Translation3f(rotation_sg * ((-1) * scene_reference_point)) *
            rotation_sg);

        // For every other point in the scene => now have pair (s_r, s_i) fixed
        std::vector<int> indices;
        std::vector<float> distances;
        scene_search_tree_->radiusSearch(target_->points[scene_reference_index],
                                         search_method_->getModelDiameter() / 2,
                                         indices, distances);
        for (size_t i = 0; i < indices.size(); ++i)
        //    for(size_t i = 0; i < target_->points.size (); ++i)
        {
            // size_t scene_point_index = i;
            size_t scene_point_index = indices[i];
            if (scene_reference_index != scene_point_index) {
                if (/*pcl::computePPFPairFeature*/ pcl::computePairFeatures(
                    target_->points[scene_reference_index].getVector4fMap(),
                    target_->points[scene_reference_index]
                        .getNormalVector4fMap(),
                    target_->points[scene_point_index].getVector4fMap(),
                    target_->points[scene_point_index].getNormalVector4fMap(),
                    f1, f2, f3, f4)) {
                    std::vector<std::pair<size_t, size_t>> nearest_indices;
                    search_method_->nearestNeighborSearch(f1, f2, f3, f4,
                                                          nearest_indices);

                    // Compute alpha_s angle
                    Eigen::Vector3f scene_point =
                        target_->points[scene_point_index].getVector3fMap();
                    Eigen::AngleAxisf rotation_sg(
                        acosf(scene_reference_normal.dot(
                            Eigen::Vector3f::UnitX())),
                        scene_reference_normal.cross(Eigen::Vector3f::UnitX())
                            .normalized());
                    Eigen::Affine3f transform_sg =
                        Eigen::Translation3f(rotation_sg *
                                             ((-1) * scene_reference_point)) *
                        rotation_sg;
                    //          float alpha_s = acos (Eigen::Vector3f::UnitY
                    //          ().dot ((transform_sg * scene_point).normalized
                    //          ()));

                    Eigen::Vector3f scene_point_transformed =
                        transform_sg * scene_point;
                    float alpha_s = atan2f(-scene_point_transformed(2),
                                           scene_point_transformed(1));
                    if (alpha_s != alpha_s) {
                        PCL_ERROR("alpha_s is nan\n");
                        continue;
                    }
                    if (sin(alpha_s) * scene_point_transformed(2) < 0.0f)
                        alpha_s *= (-1);
                    alpha_s *= (-1);

                    // Go through point pairs in the model with the same
                    // discretized feature
                    for (std::vector<std::pair<size_t, size_t>>::iterator v_it =
                             nearest_indices.begin();
                         v_it != nearest_indices.end(); ++v_it) {
                        size_t model_reference_index = v_it->first,
                               model_point_index = v_it->second;
                        // Calculate angle alpha = alpha_m - alpha_s
                        float alpha =
                            search_method_->alpha_m_[model_reference_index]
                                                    [model_point_index] -
                            alpha_s;
                        unsigned int alpha_discretized =
                            static_cast<unsigned int>(
                                floor(alpha) +
                                floor(M_PI /
                                      search_method_
                                          ->getAngleDiscretizationStep()));
                        accumulator_array[model_reference_index]
                                         [alpha_discretized]++;
                    }
                } else
                    PCL_ERROR("[pcl::PPFRegistration::computeTransformation] "
                              "Computing pair feature vector between points "
                              "%zu and %zu went wrong.\n",
                              scene_reference_index, scene_point_index);
            }
        }

        size_t max_votes_i = 0, max_votes_j = 0;
        unsigned int max_votes = 0;

        for (size_t i = 0; i < accumulator_array.size(); ++i)
            for (size_t j = 0; j < accumulator_array.back().size(); ++j) {
                if (accumulator_array[i][j] > max_votes) {
                    max_votes = accumulator_array[i][j];
                    max_votes_i = i;
                    max_votes_j = j;
                }
                // Reset accumulator_array for the next set of iterations with a
                // new scene reference point
                accumulator_array[i][j] = 0;
            }

        Eigen::Vector3f model_reference_point =
                            input_->points[max_votes_i].getVector3fMap(),
                        model_reference_normal =
                            input_->points[max_votes_i].getNormalVector3fMap();
        Eigen::AngleAxisf rotation_mg(
            acosf(model_reference_normal.dot(Eigen::Vector3f::UnitX())),
            model_reference_normal.cross(Eigen::Vector3f::UnitX())
                .normalized());
        Eigen::Affine3f transform_mg =
            Eigen::Translation3f(rotation_mg * ((-1) * model_reference_point)) *
            rotation_mg;
        Eigen::Affine3f max_transform =
            transform_sg.inverse() *
            Eigen::AngleAxisf(
                (static_cast<float>(max_votes_j) -
                 floorf(static_cast<float>(M_PI) /
                        search_method_->getAngleDiscretizationStep())) *
                    search_method_->getAngleDiscretizationStep(),
                Eigen::Vector3f::UnitX()) *
            transform_mg;

        voted_poses.push_back(PoseWithVotes(max_transform, max_votes));
    }
    PCL_DEBUG("Done with the Hough Transform ...\n");

    // Cluster poses for filtering out outliers and obtaining more precise
    // results
    PoseWithVotesList results;
    clusterPoses(voted_poses, results);

    pcl::transformPointCloud(*input_, output, results.front().pose);

    transformation_ = final_transformation_ = results.front().pose.matrix();
    converged_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void pcl::PPFRegistration<PointSource, PointTarget>::clusterPoses(
    typename pcl::PPFRegistration<PointSource, PointTarget>::PoseWithVotesList
        &poses,
    typename pcl::PPFRegistration<PointSource, PointTarget>::PoseWithVotesList
        &result) {
    PCL_INFO("Clustering poses ...\n");
    // Start off by sorting the poses by the number of votes
    sort(poses.begin(), poses.end(), poseWithVotesCompareFunction);

    std::vector<PoseWithVotesList> clusters;
    std::vector<std::pair<size_t, unsigned int>> cluster_votes;
    for (size_t poses_i = 0; poses_i < poses.size(); ++poses_i) {
        bool found_cluster = false;
        for (size_t clusters_i = 0; clusters_i < clusters.size();
             ++clusters_i) {
            if (posesWithinErrorBounds(poses[poses_i].pose,
                                       clusters[clusters_i].front().pose)) {
                found_cluster = true;
                clusters[clusters_i].push_back(poses[poses_i]);
                cluster_votes[clusters_i].second += poses[poses_i].votes;
                break;
            }
        }

        if (found_cluster == false) {
            // Create a new cluster with the current pose
            PoseWithVotesList new_cluster;
            new_cluster.push_back(poses[poses_i]);
            clusters.push_back(new_cluster);
            cluster_votes.push_back(std::pair<size_t, unsigned int>(
                clusters.size() - 1, poses[poses_i].votes));
        }
    }

    // Sort clusters by total number of votes
    std::sort(cluster_votes.begin(), cluster_votes.end(),
              clusterVotesCompareFunction);
    // Compute pose average and put them in result vector
    /// @todo some kind of threshold for determining whether a cluster has
    /// enough votes or not... now just taking the first three clusters
    result.clear();
    size_t max_clusters = (clusters.size() < 3) ? clusters.size() : 3;
    for (size_t cluster_i = 0; cluster_i < max_clusters; ++cluster_i) {
        PCL_INFO("Winning cluster has #votes: %d and #poses voted: %d.\n",
                 cluster_votes[cluster_i].second,
                 clusters[cluster_votes[cluster_i].first].size());
        Eigen::Vector3f translation_average(0.0, 0.0, 0.0);
        Eigen::Vector4f rotation_average(0.0, 0.0, 0.0, 0.0);
        for (typename PoseWithVotesList::iterator v_it =
                 clusters[cluster_votes[cluster_i].first].begin();
             v_it != clusters[cluster_votes[cluster_i].first].end(); ++v_it) {
            translation_average += v_it->pose.translation();
            /// averaging rotations by just averaging the quaternions in 4D
            /// space - reference "On Averaging Rotations" by CLAUS GRAMKOW
            rotation_average +=
                Eigen::Quaternionf(v_it->pose.rotation()).coeffs();
        }

        translation_average /=
            static_cast<float>(clusters[cluster_votes[cluster_i].first].size());
        rotation_average /=
            static_cast<float>(clusters[cluster_votes[cluster_i].first].size());

        Eigen::Affine3f transform_average;
        transform_average.translation().matrix() = translation_average;
        transform_average.linear().matrix() =
            Eigen::Quaternionf(rotation_average)
                .normalized()
                .toRotationMatrix();

        result.push_back(
            PoseWithVotes(transform_average, cluster_votes[cluster_i].second));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
bool pcl::PPFRegistration<PointSource, PointTarget>::posesWithinErrorBounds(
    Eigen::Affine3f &pose1, Eigen::Affine3f &pose2) {
    float position_diff = (pose1.translation() - pose2.translation()).norm();
    Eigen::AngleAxisf rotation_diff_mat(pose1.rotation().inverse() *
                                        pose2.rotation());

    float rotation_diff_angle = fabsf(rotation_diff_mat.angle());

    if (position_diff < clustering_position_diff_threshold_ &&
        rotation_diff_angle < clustering_rotation_diff_threshold_)
        return true;
    else
        return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
bool pcl::PPFRegistration<PointSource, PointTarget>::
    poseWithVotesCompareFunction(
        const typename pcl::PPFRegistration<PointSource,
                                            PointTarget>::PoseWithVotes &a,
        const typename pcl::PPFRegistration<PointSource,
                                            PointTarget>::PoseWithVotes &b) {
    return (a.votes > b.votes);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
bool pcl::PPFRegistration<PointSource, PointTarget>::
    clusterVotesCompareFunction(const std::pair<size_t, unsigned int> &a,
                                const std::pair<size_t, unsigned int> &b) {
    return (a.second > b.second);
}

//#define PCL_INSTANTIATE_PPFRegistration(PointSource,PointTarget) template
//class PCL_EXPORTS pcl::PPFRegistration<PointSource, PointTarget>;

#endif // PCL_REGISTRATION_IMPL_PPF_REGISTRATION_H_
