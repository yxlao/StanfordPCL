/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2010, Willow Garage, Inc.
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
 * $Id: point_cloud_handlers.hpp 6731 2012-08-07 06:16:51Z rusu $
 *
 */

#include <pcl/console/time.h>
#include <pcl/pcl_macros.h>

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerCustom<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    scalars->SetNumberOfComponents(3);

    vtkIdType nr_points = cloud_->points.size();
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);

    // Get a random color
    unsigned char *colors = new unsigned char[nr_points * 3];

    // Color every point
    for (vtkIdType cp = 0; cp < nr_points; ++cp) {
        colors[cp * 3 + 0] = static_cast<unsigned char>(r_);
        colors[cp * 3 + 1] = static_cast<unsigned char>(g_);
        colors[cp * 3 + 2] = static_cast<unsigned char>(b_);
    }
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetArray(colors, 3 * nr_points, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerRandom<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    scalars->SetNumberOfComponents(3);

    vtkIdType nr_points = cloud_->points.size();
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);

    // Get a random color
    unsigned char *colors = new unsigned char[nr_points * 3];
    double r, g, b;
    pcl::visualization::getRandomColors(r, g, b);

    int r_ = static_cast<int>(pcl_lrint(r * 255.0)),
        g_ = static_cast<int>(pcl_lrint(g * 255.0)),
        b_ = static_cast<int>(pcl_lrint(b * 255.0));

    // Color every point
    for (vtkIdType cp = 0; cp < nr_points; ++cp) {
        colors[cp * 3 + 0] = static_cast<unsigned char>(r_);
        colors[cp * 3 + 1] = static_cast<unsigned char>(g_);
        colors[cp * 3 + 2] = static_cast<unsigned char>(b_);
    }
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetArray(colors, 3 * nr_points, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudColorHandlerRGBField<
    PointT>::PointCloudColorHandlerRGBField(const PointCloudConstPtr &cloud)
    : pcl::visualization::PointCloudColorHandler<
          PointT>::PointCloudColorHandler(cloud) {
    // Handle the 24-bit packed RGB values
    field_idx_ = pcl::getFieldIndex(*cloud, "rgb", fields_);
    if (field_idx_ != -1) {
        capable_ = true;
        return;
    } else {
        field_idx_ = pcl::getFieldIndex(*cloud, "rgba", fields_);
        if (field_idx_ != -1)
            capable_ = true;
        else
            capable_ = false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerRGBField<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    scalars->SetNumberOfComponents(3);

    vtkIdType nr_points = cloud_->points.size();
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);
    unsigned char *colors =
        reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))->GetPointer(0);

    int j = 0;
    // If XYZ present, check if the points are invalid
    int x_idx = -1;
    for (size_t d = 0; d < fields_.size(); ++d)
        if (fields_[d].name == "x")
            x_idx = static_cast<int>(d);

    if (x_idx != -1) {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            // Copy the value at the specified field
            if (!pcl_isfinite(cloud_->points[cp].x) ||
                !pcl_isfinite(cloud_->points[cp].y) ||
                !pcl_isfinite(cloud_->points[cp].z))
                continue;

            int idx = j * 3;
            colors[idx] = cloud_->points[cp].r;
            colors[idx + 1] = cloud_->points[cp].g;
            colors[idx + 2] = cloud_->points[cp].b;
            j++;
        }
    } else {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            int idx = static_cast<int>(cp) * 3;
            colors[idx] = cloud_->points[cp].r;
            colors[idx + 1] = cloud_->points[cp].g;
            colors[idx + 2] = cloud_->points[cp].b;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudColorHandlerRGBCloud<
    PointT>::PointCloudColorHandlerRGBCloud(const PointCloudConstPtr &cloud,
                                            const RgbCloudConstPtr &colors)
    : PointCloudColorHandler<PointT>(cloud), rgb_(colors) {
    capable_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerRGBCloud<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    scalars->SetNumberOfComponents(3);

    vtkIdType nr_points = vtkIdType(cloud_->points.size());
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);
    unsigned char *colors =
        reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))->GetPointer(0);

    // Color every point
    if (nr_points != int(rgb_->points.size()))
        std::fill(colors, colors + nr_points * 3,
                  static_cast<unsigned char>(0xFF));
    else
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            int idx = cp * 3;
            colors[idx + 0] = rgb_->points[cp].r;
            colors[idx + 1] = rgb_->points[cp].g;
            colors[idx + 2] = rgb_->points[cp].b;
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudColorHandlerHSVField<
    PointT>::PointCloudColorHandlerHSVField(const PointCloudConstPtr &cloud)
    : pcl::visualization::PointCloudColorHandler<
          PointT>::PointCloudColorHandler(cloud) {
    // Check for the presence of the "H" field
    field_idx_ = pcl::getFieldIndex(*cloud, "h", fields_);
    if (field_idx_ == -1) {
        capable_ = false;
        return;
    }

    // Check for the presence of the "S" field
    s_field_idx_ = pcl::getFieldIndex(*cloud, "s", fields_);
    if (s_field_idx_ == -1) {
        capable_ = false;
        return;
    }

    // Check for the presence of the "V" field
    v_field_idx_ = pcl::getFieldIndex(*cloud, "v", fields_);
    if (v_field_idx_ == -1) {
        capable_ = false;
        return;
    }
    capable_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerHSVField<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    scalars->SetNumberOfComponents(3);

    vtkIdType nr_points = cloud_->points.size();
    reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);
    unsigned char *colors =
        reinterpret_cast<vtkUnsignedCharArray *>(&(*scalars))->GetPointer(0);

    int j = 0;
    // If XYZ present, check if the points are invalid
    int x_idx = -1;

    for (size_t d = 0; d < fields_.size(); ++d)
        if (fields_[d].name == "x")
            x_idx = static_cast<int>(d);

    if (x_idx != -1) {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            // Copy the value at the specified field
            if (!pcl_isfinite(cloud_->points[cp].x) ||
                !pcl_isfinite(cloud_->points[cp].y) ||
                !pcl_isfinite(cloud_->points[cp].z))
                continue;

            int idx = j * 3;

            ///@todo do this with the point_types_conversion in common, first
            ///template it!

            // Fill color data with HSV here:
            if (cloud_->points[cp].s == 0) {
                colors[idx] = colors[idx + 1] = colors[idx + 2] =
                    cloud_->points[cp].v;
                return;
            }
            float a = cloud_->points[cp].h / 60;
            int i = floor(a);
            float f = a - i;
            float p = cloud_->points[cp].v * (1 - cloud_->points[cp].s);
            float q = cloud_->points[cp].v * (1 - cloud_->points[cp].s * f);
            float t =
                cloud_->points[cp].v * (1 - cloud_->points[cp].s * (1 - f));

            switch (i) {
            case 0:
                colors[idx] = cloud_->points[cp].v;
                colors[idx + 1] = t;
                colors[idx + 2] = p;
                break;
            case 1:
                colors[idx] = q;
                colors[idx + 1] = cloud_->points[cp].v;
                colors[idx + 2] = p;
                break;
            case 2:
                colors[idx] = p;
                colors[idx + 1] = cloud_->points[cp].v;
                colors[idx + 2] = t;
                break;
            case 3:
                colors[idx] = p;
                colors[idx + 1] = q;
                colors[idx + 2] = cloud_->points[cp].v;
                break;
            case 4:
                colors[idx] = t;
                colors[idx + 1] = p;
                colors[idx + 2] = cloud_->points[cp].v;
                break;
            default:
                colors[idx] = cloud_->points[cp].v;
                colors[idx + 1] = p;
                colors[idx + 2] = q;
                break;
            }
            j++;
        }
    } else {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            int idx = cp * 3;

            // Fill color data with HSV here:
            if (cloud_->points[cp].s == 0) {
                colors[idx] = colors[idx + 1] = colors[idx + 2] =
                    cloud_->points[cp].v;
                return;
            }
            float a = cloud_->points[cp].h / 60;
            int i = floor(a);
            float f = a - i;
            float p = cloud_->points[cp].v * (1 - cloud_->points[cp].s);
            float q = cloud_->points[cp].v * (1 - cloud_->points[cp].s * f);
            float t =
                cloud_->points[cp].v * (1 - cloud_->points[cp].s * (1 - f));

            switch (i) {
            case 0:
                colors[idx] = cloud_->points[cp].v;
                colors[idx + 1] = t;
                colors[idx + 2] = p;
                break;
            case 1:
                colors[idx] = q;
                colors[idx + 1] = cloud_->points[cp].v;
                colors[idx + 2] = p;
                break;
            case 2:
                colors[idx] = p;
                colors[idx + 1] = cloud_->points[cp].v;
                colors[idx + 2] = t;
                break;
            case 3:
                colors[idx] = p;
                colors[idx + 1] = q;
                colors[idx + 2] = cloud_->points[cp].v;
                break;
            case 4:
                colors[idx] = t;
                colors[idx + 1] = p;
                colors[idx + 2] = cloud_->points[cp].v;
                break;
            default:
                colors[idx] = cloud_->points[cp].v;
                colors[idx + 1] = p;
                colors[idx + 2] = q;
                break;
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudColorHandlerGenericField<
    PointT>::PointCloudColorHandlerGenericField(const PointCloudConstPtr &cloud,
                                                const std::string &field_name)
    : pcl::visualization::PointCloudColorHandler<
          PointT>::PointCloudColorHandler(cloud),
      field_name_(field_name) {
    field_idx_ = pcl::getFieldIndex(*cloud, field_name, fields_);
    if (field_idx_ != -1)
        capable_ = true;
    else
        capable_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudColorHandlerGenericField<PointT>::getColor(
    vtkSmartPointer<vtkDataArray> &scalars) const {
    if (!capable_)
        return;

    if (!scalars)
        scalars = vtkSmartPointer<vtkFloatArray>::New();
    scalars->SetNumberOfComponents(1);

    vtkIdType nr_points = cloud_->points.size();
    reinterpret_cast<vtkFloatArray *>(&(*scalars))
        ->SetNumberOfTuples(nr_points);

    typedef typename pcl::traits::fieldList<PointT>::type FieldList;

    float *colors = new float[nr_points];
    float field_data;

    int j = 0;
    // If XYZ present, check if the points are invalid
    int x_idx = -1;
    for (size_t d = 0; d < fields_.size(); ++d)
        if (fields_[d].name == "x")
            x_idx = static_cast<int>(d);

    if (x_idx != -1) {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            // Copy the value at the specified field
            if (!pcl_isfinite(cloud_->points[cp].x) ||
                !pcl_isfinite(cloud_->points[cp].y) ||
                !pcl_isfinite(cloud_->points[cp].z))
                continue;

            const uint8_t *pt_data =
                reinterpret_cast<const uint8_t *>(&cloud_->points[cp]);
            memcpy(&field_data, pt_data + fields_[field_idx_].offset,
                   pcl::getFieldSize(fields_[field_idx_].datatype));

            colors[j] = field_data;
            j++;
        }
    } else {
        // Color every point
        for (vtkIdType cp = 0; cp < nr_points; ++cp) {
            const uint8_t *pt_data =
                reinterpret_cast<const uint8_t *>(&cloud_->points[cp]);
            memcpy(&field_data, pt_data + fields_[field_idx_].offset,
                   pcl::getFieldSize(fields_[field_idx_].datatype));

            if (!pcl_isfinite(field_data))
                continue;

            colors[j] = field_data;
            j++;
        }
    }
    reinterpret_cast<vtkFloatArray *>(&(*scalars))->SetArray(colors, j, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudGeometryHandlerXYZ<
    PointT>::PointCloudGeometryHandlerXYZ(const PointCloudConstPtr &cloud)
    : pcl::visualization::PointCloudGeometryHandler<
          PointT>::PointCloudGeometryHandler(cloud) {
    field_x_idx_ = pcl::getFieldIndex(*cloud, "x", fields_);
    if (field_x_idx_ == -1)
        return;
    field_y_idx_ = pcl::getFieldIndex(*cloud, "y", fields_);
    if (field_y_idx_ == -1)
        return;
    field_z_idx_ = pcl::getFieldIndex(*cloud, "z", fields_);
    if (field_z_idx_ == -1)
        return;
    capable_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudGeometryHandlerXYZ<PointT>::getGeometry(
    vtkSmartPointer<vtkPoints> &points) const {
    if (!capable_)
        return;

    if (!points)
        points = vtkSmartPointer<vtkPoints>::New();
    points->SetDataTypeToFloat();

    vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
    data->SetNumberOfComponents(3);
    vtkIdType nr_points = cloud_->points.size();

    // Add all points
    vtkIdType j = 0; // true point index
    float *pts = static_cast<float *>(malloc(nr_points * 3 * sizeof(float)));

    // If the dataset has no invalid values, just copy all of them
    if (cloud_->is_dense) {
        for (vtkIdType i = 0; i < nr_points; ++i) {
            pts[i * 3 + 0] = cloud_->points[i].x;
            pts[i * 3 + 1] = cloud_->points[i].y;
            pts[i * 3 + 2] = cloud_->points[i].z;
        }
        data->SetArray(&pts[0], nr_points * 3, 0);
        points->SetData(data);
    }
    // Need to check for NaNs, Infs, ec
    else {
        for (vtkIdType i = 0; i < nr_points; ++i) {
            // Check if the point is invalid
            if (!pcl_isfinite(cloud_->points[i].x) ||
                !pcl_isfinite(cloud_->points[i].y) ||
                !pcl_isfinite(cloud_->points[i].z))
                continue;

            pts[j * 3 + 0] = cloud_->points[i].x;
            pts[j * 3 + 1] = cloud_->points[i].y;
            pts[j * 3 + 2] = cloud_->points[i].z;
            // Set j and increment
            j++;
        }
        data->SetArray(&pts[0], j * 3, 0);
        points->SetData(data);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudGeometryHandlerSurfaceNormal<PointT>::
    PointCloudGeometryHandlerSurfaceNormal(const PointCloudConstPtr &cloud)
    : pcl::visualization::PointCloudGeometryHandler<
          PointT>::PointCloudGeometryHandler(cloud) {
    field_x_idx_ = pcl::getFieldIndex(*cloud, "normal_x", fields_);
    if (field_x_idx_ == -1)
        return;
    field_y_idx_ = pcl::getFieldIndex(*cloud, "normal_y", fields_);
    if (field_y_idx_ == -1)
        return;
    field_z_idx_ = pcl::getFieldIndex(*cloud, "normal_z", fields_);
    if (field_z_idx_ == -1)
        return;
    capable_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudGeometryHandlerSurfaceNormal<
    PointT>::getGeometry(vtkSmartPointer<vtkPoints> &points) const {
    if (!capable_)
        return;

    if (!points)
        points = vtkSmartPointer<vtkPoints>::New();
    points->SetDataTypeToFloat();
    points->SetNumberOfPoints(cloud_->points.size());

    // Add all points
    double p[3];
    for (vtkIdType i = 0; i < static_cast<vtkIdType>(cloud_->points.size());
         ++i) {
        p[0] = cloud_->points[i].normal[0];
        p[1] = cloud_->points[i].normal[1];
        p[2] = cloud_->points[i].normal[2];

        points->SetPoint(i, p);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
pcl::visualization::PointCloudGeometryHandlerCustom<
    PointT>::PointCloudGeometryHandlerCustom(const PointCloudConstPtr &cloud,
                                             const std::string &x_field_name,
                                             const std::string &y_field_name,
                                             const std::string &z_field_name)
    : pcl::visualization::PointCloudGeometryHandler<
          PointT>::PointCloudGeometryHandler(cloud) {
    field_x_idx_ = pcl::getFieldIndex(*cloud, x_field_name, fields_);
    if (field_x_idx_ == -1)
        return;
    field_y_idx_ = pcl::getFieldIndex(*cloud, y_field_name, fields_);
    if (field_y_idx_ == -1)
        return;
    field_z_idx_ = pcl::getFieldIndex(*cloud, z_field_name, fields_);
    if (field_z_idx_ == -1)
        return;
    field_name_ = x_field_name + y_field_name + z_field_name;
    capable_ = true;
}

////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void pcl::visualization::PointCloudGeometryHandlerCustom<PointT>::getGeometry(
    vtkSmartPointer<vtkPoints> &points) const {
    if (!capable_)
        return;

    if (!points)
        points = vtkSmartPointer<vtkPoints>::New();
    points->SetDataTypeToFloat();
    points->SetNumberOfPoints(cloud_->points.size());

    float data;
    // Add all points
    double p[3];
    for (vtkIdType i = 0; i < static_cast<vtkIdType>(cloud_->points.size());
         ++i) {
        // Copy the value at the specified field
        const uint8_t *pt_data =
            reinterpret_cast<const uint8_t *>(&cloud_->points[i]);
        memcpy(&data, pt_data + fields_[field_x_idx_].offset, sizeof(float));
        p[0] = data;

        memcpy(&data, pt_data + fields_[field_y_idx_].offset, sizeof(float));
        p[1] = data;

        memcpy(&data, pt_data + fields_[field_z_idx_].offset, sizeof(float));
        p[2] = data;

        points->SetPoint(i, p);
    }
}
