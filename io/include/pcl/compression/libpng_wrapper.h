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
 * Author: Julius Kammerl (julius@kammerl.de)
 */

#ifndef __PCL_IO_LIBPNG_WRAPPER__
#define __PCL_IO_LIBPNG_WRAPPER__

#include <vector>
#include <pcl/common/common.h>
#include <pcl/common/io.h>

namespace pcl {
namespace io {
/** \brief Encodes 8-bit mono image to PNG format.
 * \param[in] image_arg input image data
 * \param[in] width_arg image width
 * \param[in] height_arg image height
 * \param[out] pngData PNG compressed image data
 * \param[in] png_level_arg zLib compression level (default level: -1)
 * \ingroup io
 */
PCL_EXPORTS void encodeMonoImageToPNG(std::vector<uint8_t> &image_arg,
                                      size_t width_arg, size_t height_arg,
                                      std::vector<uint8_t> &pngData_arg,
                                      int png_level_arg = -1);

/** \brief Encodes 16-bit mono image to PNG format.
 * \param[in] image_arg input image data
 * \param[in] width_arg image width
 * \param[in] height_arg image height
 * \param[out] pngData PNG compressed image data
 * \param[in] png_level_arg zLib compression level (default level: -1)
 * \ingroup io
 */
PCL_EXPORTS void encodeMonoImageToPNG(std::vector<uint16_t> &image_arg,
                                      size_t width_arg, size_t height_arg,
                                      std::vector<uint8_t> &pngData_arg,
                                      int png_level_arg = -1);

/** \brief Encodes 8-bit RGB image to PNG format.
 * \param[in] image_arg input image data
 * \param[in] width_arg image width
 * \param[in] height_arg image height
 * \param[out] pngData PNG compressed image data
 * \param[in] png_level_arg zLib compression level (default level: -1)
 * \ingroup io
 */
PCL_EXPORTS void encodeRGBImageToPNG(std::vector<uint8_t> &image_arg,
                                     size_t width_arg, size_t height_arg,
                                     std::vector<uint8_t> &pngData_arg,
                                     int png_level_arg = -1);

/** \brief Encodes 16-bit RGB image to PNG format.
 * \param[in] image_arg input image data
 * \param[in] width_arg image width
 * \param[in] height_arg image height
 * \param[out] pngData PNG compressed image data
 * \param[in] png_level_arg zLib compression level (default level: -1)
 * \ingroup io
 */
PCL_EXPORTS void encodeRGBImageToPNG(std::vector<uint16_t> &image_arg,
                                     size_t width_arg, size_t height_arg,
                                     std::vector<uint8_t> &pngData_arg,
                                     int png_level_arg = -1);

/** \brief Decode compressed PNG to 8-bit image
 * \param[in] pngData PNG compressed input data
 * \param[in] imageData image output data
 * \param[out] width image width
 * \param[out] height image height
 * \param[out] channels number of channels
 * \ingroup io
 */
PCL_EXPORTS void decodePNGToImage(std::vector<uint8_t> &pngData_arg,
                                  std::vector<uint8_t> &imageData_arg,
                                  size_t &width_arg, size_t &heigh_argt,
                                  unsigned int &channels_arg);

/** \brief Decode compressed PNG to 16-bit image
 * \param[in] pngData PNG compressed input data
 * \param[in] imageData image output data
 * \param[out] width image width
 * \param[out] height image height
 * \param[out] channels number of channels
 * \ingroup io
 */
PCL_EXPORTS void decodePNGToImage(std::vector<uint8_t> &pngData_arg,
                                  std::vector<uint16_t> &imageData_arg,
                                  size_t &width_arg, size_t &height_arg,
                                  unsigned int &channels_arg);
} // namespace io
} // namespace pcl

#endif
