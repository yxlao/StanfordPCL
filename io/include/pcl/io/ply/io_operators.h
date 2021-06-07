/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2007-2012, Ares Lagae
 *  Copyright (c) 2012, Willow Garage, Inc.
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
 * $Id: io_operators.h 6459 2012-07-18 07:50:37Z dpb $
 *
 */

#ifndef PCL_IO_PLY_IO_OPERATORS_H
#define PCL_IO_PLY_IO_OPERATORS_H

#include <istream>
#include <limits>
#include <ostream>
#include <stdint.h>

namespace pcl {
namespace io {
namespace ply {
/** \file io_operators.h
 * defines output operators for int8_t and uint8_t
 * \author Ares Lagae as part of libply, Nizar Sallem
 * \ingroup io
 */
namespace io_operators {

inline std::istream &operator>>(std::istream &istream, int8_t &value) {
    int16_t tmp;
    if (istream >> tmp) {
        if (tmp <= std::numeric_limits<int8_t>::max())
            value = static_cast<int8_t>(tmp);
        else
            istream.setstate(std::ios_base::failbit);
    }
    return (istream);
}

inline std::istream &operator>>(std::istream &istream, uint8_t &value) {
    uint16_t tmp;
    if (istream >> tmp) {
        if (tmp <= std::numeric_limits<uint8_t>::max())
            value = static_cast<uint8_t>(tmp);
        else
            istream.setstate(std::ios_base::failbit);
    }
    return (istream);
}

inline std::ostream &operator<<(std::ostream &ostream, int8_t value) {
    return (ostream << static_cast<int16_t>(value));
}

inline std::ostream &operator<<(std::ostream &ostream, uint8_t value) {
    return (ostream << static_cast<uint16_t>(value));
}

} // namespace io_operators
} // namespace ply
} // namespace io
} // namespace pcl

#endif // PLY_IO_OPERATORS_H
