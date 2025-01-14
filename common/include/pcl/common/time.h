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
 */

#ifndef PCL_TIME_H_
#define PCL_TIME_H_

#include <cmath>
#include <string>
#include <pcl/common/boost.h>

/**
 * \file pcl/common/time.h
 * Define methods for measuring time spent in code blocks
 * \ingroup common
 */

/*@{*/
namespace pcl {

/** \brief Simple stopwatch.
 * \ingroup common
 */
class StopWatch {
  public:
    /** \brief Constructor. */
    StopWatch()
        : start_time_(boost::posix_time::microsec_clock::local_time()) {}

    /** \brief Destructor. */
    virtual ~StopWatch() {}

    /** \brief Retrieve the time in milliseconds spent since the last call to \a
     * reset(). */
    inline double getTime() {
        boost::posix_time::ptime end_time =
            boost::posix_time::microsec_clock::local_time();
        return (static_cast<double>(
            ((end_time - start_time_).total_milliseconds())));
    }

    /** \brief Retrieve the time in seconds spent since the last call to \a
     * reset(). */
    inline double getTimeSeconds() { return (getTime() * 0.001f); }

    /** \brief Reset the stopwatch to 0. */
    inline void reset() {
        start_time_ = boost::posix_time::microsec_clock::local_time();
    }

  protected:
    boost::posix_time::ptime start_time_;
};

/** \brief Class to measure the time spent in a scope
 *
 * To use this class, e.g. to measure the time spent in a function,
 * just create an instance at the beginning of the function. Example:
 *
 * \code
 * {
 *   pcl::ScopeTime t1 ("calculation");
 *
 *   // ... perform calculation here
 * }
 * \endcode
 *
 * \ingroup common
 */
class ScopeTime : public StopWatch {
  public:
    inline ScopeTime(const char *title) : title_(std::string(title)) {
        start_time_ = boost::posix_time::microsec_clock::local_time();
    }

    inline ScopeTime() : title_(std::string("")) {
        start_time_ = boost::posix_time::microsec_clock::local_time();
    }

    inline ~ScopeTime() {
        double val = this->getTime();
        std::cerr << title_ << " took " << val << "ms.\n";
    }

  private:
    std::string title_;
};

#ifndef MEASURE_FUNCTION_TIME
#define MEASURE_FUNCTION_TIME ScopeTime scopeTime(__func__)
#endif

inline double getTime() {
    boost::posix_time::ptime epoch_time(boost::gregorian::date(1970, 1, 1));
    boost::posix_time::ptime current_time =
        boost::posix_time::microsec_clock::local_time();
    return (
        static_cast<double>((current_time - epoch_time).total_nanoseconds()) *
        1.0e-9);
}

/// Executes code, only if secs are gone since last exec.
#ifndef DO_EVERY_TS
#define DO_EVERY_TS(secs, currentTime, code)                                   \
    if (1) {                                                                   \
        static double s_lastDone_ = 0.0;                                       \
        double s_now_ = (currentTime);                                         \
        if (s_lastDone_ > s_now_)                                              \
            s_lastDone_ = s_now_;                                              \
        if ((s_now_ - s_lastDone_) > (secs)) {                                 \
            code;                                                              \
            s_lastDone_ = s_now_;                                              \
        }                                                                      \
    } else                                                                     \
        (void)0
#endif

/// Executes code, only if secs are gone since last exec.
#ifndef DO_EVERY
#define DO_EVERY(secs, code) DO_EVERY_TS(secs, pcl::getTime(), code)
#endif

} // namespace pcl
/*@}*/

#endif //#ifndef PCL_NORMS_H_
