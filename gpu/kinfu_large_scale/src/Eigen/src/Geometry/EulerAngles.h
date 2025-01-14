// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008 Gael Guennebaud <gael.guennebaud@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef EIGEN_EULERANGLES_H
#define EIGEN_EULERANGLES_H

namespace Eigen {

/** \geometry_module \ingroup Geometry_Module
 *
 *
 * \returns the Euler-angles of the rotation matrix \c *this using the
 * convention defined by the triplet (\a a0,\a a1,\a a2)
 *
 * Each of the three parameters \a a0,\a a1,\a a2 represents the respective
 * rotation axis as an integer in {0,1,2}. For instance, in: \code Vector3f ea =
 * mat.eulerAngles(2, 0, 2); \endcode "2" represents the z axis and "0" the x
 * axis, etc. The returned angles are such that we have the following equality:
 * \code
 * mat == AngleAxisf(ea[0], Vector3f::UnitZ())
 *      * AngleAxisf(ea[1], Vector3f::UnitX())
 *      * AngleAxisf(ea[2], Vector3f::UnitZ()); \endcode
 * This corresponds to the right-multiply conventions (with right hand side
 * frames).
 */
template <typename Derived>
inline Matrix<typename MatrixBase<Derived>::Scalar, 3, 1>
MatrixBase<Derived>::eulerAngles(Index a0, Index a1, Index a2) const {
    /* Implemented from Graphics Gems IV */
    EIGEN_STATIC_ASSERT_MATRIX_SPECIFIC_SIZE(Derived, 3, 3)

    Matrix<Scalar, 3, 1> res;
    typedef Matrix<typename Derived::Scalar, 2, 1> Vector2;
    const Scalar epsilon = NumTraits<Scalar>::dummy_precision();

    const Index odd = ((a0 + 1) % 3 == a1) ? 0 : 1;
    const Index i = a0;
    const Index j = (a0 + 1 + odd) % 3;
    const Index k = (a0 + 2 - odd) % 3;

    if (a0 == a2) {
        Scalar s = Vector2(coeff(j, i), coeff(k, i)).norm();
        res[1] = internal::atan2(s, coeff(i, i));
        if (s > epsilon) {
            res[0] = internal::atan2(coeff(j, i), coeff(k, i));
            res[2] = internal::atan2(coeff(i, j), -coeff(i, k));
        } else {
            res[0] = Scalar(0);
            res[2] = (coeff(i, i) > 0 ? 1 : -1) *
                     internal::atan2(-coeff(k, j), coeff(j, j));
        }
    } else {
        Scalar c = Vector2(coeff(i, i), coeff(i, j)).norm();
        res[1] = internal::atan2(-coeff(i, k), c);
        if (c > epsilon) {
            res[0] = internal::atan2(coeff(j, k), coeff(k, k));
            res[2] = internal::atan2(coeff(i, j), coeff(i, i));
        } else {
            res[0] = Scalar(0);
            res[2] = (coeff(i, k) > 0 ? 1 : -1) *
                     internal::atan2(-coeff(k, j), coeff(j, j));
        }
    }
    if (!odd)
        res = -res;
    return res;
}

} // end namespace Eigen

#endif // EIGEN_EULERANGLES_H
