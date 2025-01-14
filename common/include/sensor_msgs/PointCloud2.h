#ifndef PCL_SENSOR_MSGS_MESSAGE_POINTCLOUD2_H
#define PCL_SENSOR_MSGS_MESSAGE_POINTCLOUD2_H
#include <string>
#include <vector>
#include <ostream>
#include <boost/detail/endian.hpp>

// Include the correct Header path here
#include "std_msgs/Header.h"
#include "sensor_msgs/PointField.h"

namespace sensor_msgs {

struct PointCloud2 {
    PointCloud2()
        : header(), height(0), width(0), fields(), is_bigendian(false),
          point_step(0), row_step(0), data(), is_dense(false) {
#if defined(BOOST_BIG_ENDIAN)
        is_bigendian = true;
#elif defined(BOOST_LITTLE_ENDIAN)
        is_bigendian = false;
#else
#error "unable to determine system endianness"
#endif
    }

    ::std_msgs::Header header;

    pcl::uint32_t height;
    pcl::uint32_t width;

    std::vector<::sensor_msgs::PointField> fields;

    pcl::uint8_t is_bigendian;
    pcl::uint32_t point_step;
    pcl::uint32_t row_step;

    std::vector<pcl::uint8_t> data;

    pcl::uint8_t is_dense;

  public:
    typedef boost::shared_ptr<::sensor_msgs::PointCloud2> Ptr;
    typedef boost::shared_ptr<::sensor_msgs::PointCloud2 const> ConstPtr;
}; // struct PointCloud2

typedef boost::shared_ptr<::sensor_msgs::PointCloud2> PointCloud2Ptr;
typedef boost::shared_ptr<::sensor_msgs::PointCloud2 const> PointCloud2ConstPtr;

inline std::ostream &operator<<(std::ostream &s,
                                const ::sensor_msgs::PointCloud2 &v) {
    s << "header: " << std::endl;
    s << v.header;
    s << "height: ";
    s << "  " << v.height << std::endl;
    s << "width: ";
    s << "  " << v.width << std::endl;
    s << "fields[]" << std::endl;
    for (size_t i = 0; i < v.fields.size(); ++i) {
        s << "  fields[" << i << "]: ";
        s << std::endl;
        s << "    " << v.fields[i] << std::endl;
    }
    s << "is_bigendian: ";
    s << "  " << v.is_bigendian << std::endl;
    s << "point_step: ";
    s << "  " << v.point_step << std::endl;
    s << "row_step: ";
    s << "  " << v.row_step << std::endl;
    s << "data[]" << std::endl;
    for (size_t i = 0; i < v.data.size(); ++i) {
        s << "  data[" << i << "]: ";
        s << "  " << v.data[i] << std::endl;
    }
    s << "is_dense: ";
    s << "  " << v.is_dense << std::endl;

    return (s);
}

} // namespace sensor_msgs

#endif // PCL_SENSOR_MSGS_MESSAGE_POINTCLOUD2_H
