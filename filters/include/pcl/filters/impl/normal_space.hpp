/*
 * Software License Agreement (BSD License)
 *
 * Point Cloud Library (PCL) - www.pointclouds.org
 * Copyright (c) 2009-2011, Willow Garage, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Willow Garage, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef PCL_FILTERS_IMPL_NORMAL_SPACE_SAMPLE_H_
#define PCL_FILTERS_IMPL_NORMAL_SPACE_SAMPLE_H_

#include <pcl/filters/normal_space.h>

#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename NormalT>
void pcl::NormalSpaceSampling<PointT, NormalT>::applyFilter(
    PointCloud &output) {
    // If sample size is 0 or if the sample size is greater then input cloud
    // size
    //   then return entire copy of cloud
    if (sample_ >= input_->size()) {
        output = *input_;
        return;
    }

    // Resize output cloud to sample size
    output.points.resize(sample_);
    output.width = sample_;
    output.height = 1;

    // allocate memory for the histogram of normals.. Normals will then be
    // sampled from each bin..
    unsigned int n_bins = binsx_ * binsy_ * binsz_;
    // list<int> holds the indices of points in that bin.. Using list to avoid
    // repeated resizing of vectors.. Helps when the point cloud is large..
    std::vector<std::list<int>> normals_hg;
    normals_hg.reserve(n_bins);
    for (unsigned int i = 0; i < n_bins; i++)
        normals_hg.push_back(std::list<int>());

    for (unsigned int i = 0; i < input_normals_->points.size(); i++) {
        unsigned int bin_number =
            findBin(input_normals_->points[i].normal, n_bins);
        normals_hg[bin_number].push_back(i);
    }

    // Setting up random access for the list created above.. Maintaining the
    // iterators to individual elements of the list in a vector.. Using vector
    // now as the size of the list is known..
    std::vector<std::vector<std::list<int>::iterator>> random_access(
        normals_hg.size());
    for (unsigned int i = 0; i < normals_hg.size(); i++) {
        random_access.push_back(std::vector<std::list<int>::iterator>());
        random_access[i].resize(normals_hg[i].size());

        unsigned int j = 0;
        for (std::list<int>::iterator itr = normals_hg[i].begin();
             itr != normals_hg[i].end(); itr++, j++) {
            random_access[i][j] = itr;
        }
    }
    unsigned int *start_index = new unsigned int[normals_hg.size()];
    start_index[0] = 0;
    unsigned int prev_index = start_index[0];
    for (unsigned int i = 1; i < normals_hg.size(); i++) {
        start_index[i] =
            prev_index + static_cast<unsigned int>(normals_hg[i - 1].size());
        prev_index = start_index[i];
    }

    // maintaining flags to check if a point is sampled
    boost::dynamic_bitset<> is_sampled_flag(input_normals_->points.size());
    // maintaining flags to check if all points in the bin are sampled
    boost::dynamic_bitset<> bin_empty_flag(normals_hg.size());
    unsigned int i = 0;
    while (i < sample_) {
        // iterating through every bin and picking one point at random, until
        // the required number of points are sampled..
        for (unsigned int j = 0; j < normals_hg.size(); j++) {
            unsigned int M = static_cast<unsigned int>(normals_hg[j].size());
            if (M == 0 ||
                bin_empty_flag.test(j)) // bin_empty_flag(i) is set if all
                                        // points in that bin are sampled..
            {
                continue;
            }

            unsigned int pos = 0;
            unsigned int random_index = 0;
            // picking up a sample at random from jth bin
            do {
                random_index = std::rand() % M;
                pos = start_index[j] + random_index;
            } while (is_sampled_flag.test(pos));

            is_sampled_flag.flip(start_index[j] + random_index);

            // checking if all points in bin j are sampled..
            if (isEntireBinSampled(
                    is_sampled_flag, start_index[j],
                    static_cast<unsigned int>(normals_hg[j].size()))) {
                bin_empty_flag.flip(j);
            }

            unsigned int index = *(random_access[j][random_index]);
            output.points[i] = input_->points[index];
            i++;
            if (i == sample_) {
                break;
            }
        }
    }
    delete[] start_index;
}

///////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename NormalT>
bool pcl::NormalSpaceSampling<PointT, NormalT>::isEntireBinSampled(
    boost::dynamic_bitset<> &array, unsigned int start_index,
    unsigned int length) {
    bool status = true;
    for (unsigned int i = start_index; i < start_index + length; i++) {
        status = status & array.test(i);
    }
    return status;
}

///////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename NormalT>
unsigned int pcl::NormalSpaceSampling<PointT, NormalT>::findBin(float *normal,
                                                                unsigned int) {
    unsigned int bin_number = 0;
    // holds the bin numbers for direction cosines in x,y,z directions
    unsigned int t[3] = {0, 0, 0};

    // dcos is the direction cosine.
    float dcos = 0.0;
    float bin_size = 0.0;
    // max_cos and min_cos are the maximum and minimum values of cos(theta)
    // respectively
    float max_cos = 1.0;
    float min_cos = -1.0;

    dcos = cosf(normal[0]);
    bin_size = (max_cos - min_cos) / static_cast<float>(binsx_);

    // finding bin number for direction cosine in x direction
    unsigned int k = 0;
    for (float i = min_cos; (i + bin_size) < (max_cos - bin_size);
         i += bin_size, k++) {
        if (dcos >= i && dcos <= (i + bin_size)) {
            break;
        }
    }
    t[0] = k;

    dcos = cosf(normal[1]);
    bin_size = (max_cos - min_cos) / static_cast<float>(binsy_);

    // finding bin number for direction cosine in y direction
    k = 0;
    for (float i = min_cos; (i + bin_size) < (max_cos - bin_size);
         i += bin_size, k++) {
        if (dcos >= i && dcos <= (i + bin_size)) {
            break;
        }
    }
    t[1] = k;

    dcos = cosf(normal[2]);
    bin_size = (max_cos - min_cos) / static_cast<float>(binsz_);

    // finding bin number for direction cosine in z direction
    k = 0;
    for (float i = min_cos; (i + bin_size) < (max_cos - bin_size);
         i += bin_size, k++) {
        if (dcos >= i && dcos <= (i + bin_size)) {
            break;
        }
    }
    t[2] = k;

    bin_number = t[0] * (binsy_ * binsz_) + t[1] * binsz_ + t[2];
    return bin_number;
}

///////////////////////////////////////////////////////////////////////////////
template <typename PointT, typename NormalT>
void pcl::NormalSpaceSampling<PointT, NormalT>::applyFilter(
    std::vector<int> &indices) {
    // If sample size is 0 or if the sample size is greater then input cloud
    // size
    //   then return all indices
    if (sample_ >= input_->width * input_->height) {
        indices = *indices_;
        return;
    }

    // Resize output indices to sample size
    indices.resize(sample_);

    // allocate memory for the histogram of normals.. Normals will then be
    // sampled from each bin..
    unsigned int n_bins = binsx_ * binsy_ * binsz_;
    // list<int> holds the indices of points in that bin.. Using list to avoid
    // repeated resizing of vectors.. Helps when the point cloud is large..
    std::vector<std::list<int>> normals_hg;
    normals_hg.reserve(n_bins);
    for (unsigned int i = 0; i < n_bins; i++)
        normals_hg.push_back(std::list<int>());

    for (unsigned int i = 0; i < input_normals_->points.size(); i++) {
        unsigned int bin_number =
            findBin(input_normals_->points[i].normal, n_bins);
        normals_hg[bin_number].push_back(i);
    }

    // Setting up random access for the list created above.. Maintaining the
    // iterators to individual elements of the list in a vector.. Using vector
    // now as the size of the list is known..
    std::vector<std::vector<std::list<int>::iterator>> random_access(
        normals_hg.size());
    for (unsigned int i = 0; i < normals_hg.size(); i++) {
        random_access.push_back(std::vector<std::list<int>::iterator>());
        random_access[i].resize(normals_hg[i].size());

        unsigned int j = 0;
        for (std::list<int>::iterator itr = normals_hg[i].begin();
             itr != normals_hg[i].end(); itr++, j++) {
            random_access[i][j] = itr;
        }
    }
    unsigned int *start_index = new unsigned int[normals_hg.size()];
    start_index[0] = 0;
    unsigned int prev_index = start_index[0];
    for (unsigned int i = 1; i < normals_hg.size(); i++) {
        start_index[i] =
            prev_index + static_cast<unsigned int>(normals_hg[i - 1].size());
        prev_index = start_index[i];
    }

    // maintaining flags to check if a point is sampled
    boost::dynamic_bitset<> is_sampled_flag(input_normals_->points.size());
    // maintaining flags to check if all points in the bin are sampled
    boost::dynamic_bitset<> bin_empty_flag(normals_hg.size());
    unsigned int i = 0;
    while (i < sample_) {
        // iterating through every bin and picking one point at random, until
        // the required number of points are sampled..
        for (unsigned int j = 0; j < normals_hg.size(); j++) {
            unsigned int M = static_cast<unsigned int>(normals_hg[j].size());
            if (M == 0 ||
                bin_empty_flag.test(j)) // bin_empty_flag(i) is set if all
                                        // points in that bin are sampled..
            {
                continue;
            }

            unsigned int pos = 0;
            unsigned int random_index = 0;

            // picking up a sample at random from jth bin
            do {
                random_index = std::rand() % M;
                pos = start_index[j] + random_index;
            } while (is_sampled_flag.test(pos));

            is_sampled_flag.flip(start_index[j] + random_index);

            // checking if all points in bin j are sampled..
            if (isEntireBinSampled(
                    is_sampled_flag, start_index[j],
                    static_cast<unsigned int>(normals_hg[j].size()))) {
                bin_empty_flag.flip(j);
            }

            unsigned int index = *(random_access[j][random_index]);
            indices[i] = index;
            i++;
            if (i == sample_) {
                break;
            }
        }
    }
    delete[] start_index;
}

#define PCL_INSTANTIATE_NormalSpaceSampling(T, NT)                             \
    template class PCL_EXPORTS pcl::NormalSpaceSampling<T, NT>;

#endif // PCL_FILTERS_IMPL_NORMAL_SPACE_SAMPLE_H_
