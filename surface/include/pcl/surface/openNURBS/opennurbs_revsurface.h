/* $NoKeywords: $ */
/*
//
// Copyright (c) 1993-2011 Robert McNeel & Associates. All rights reserved.
// OpenNURBS, Rhinoceros, and Rhino3D are registered trademarks of Robert
// McNeel & Associates.
//
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
// ALL IMPLIED WARRANTIES OF FITNESS FOR ANY PARTICULAR PURPOSE AND OF
// MERCHANTABILITY ARE HEREBY DISCLAIMED.
//
// For complete openNURBS copyright information see <http://www.opennurbs.org>.
//
////////////////////////////////////////////////////////////////
*/

#if !defined(OPENNURBS_REVSURFACE_INC_)
#define OPENNURBS_REVSURFACE_INC_

// surface of revolution
class ON_CLASS ON_RevSurface : public ON_Surface {
    ON_OBJECT_DECLARE(ON_RevSurface);

  public:
    // virtual ON_Object::DestroyRuntimeCache override
    void DestroyRuntimeCache(bool bDelete = true);

  public:
    // Revolute curve.
    // If m_curve is not NULL, then ~ON_RevSurface() deletes m_curve.
    ON_Curve *m_curve;

    // Axis of revolution.
    ON_Line m_axis;

    // Start and end angles of revolution in radians.
    // The interval m_angle must be increasing and satisfiy
    // ON_ZERO_TOLERANCE < m_angle.Length() <= 2.0*ON_PI
    ON_Interval m_angle;

    // The interval m_t specifies the parameterization for the
    // angular parameter; m_t must be an increasing interval.
    // The parameter m_t[0] corresonds to angle m_angle[0] and
    // the parameter m_t[1] corresponds to angle m_angle[1].
    // Changing m_t and leaving m_angle unchanged will change the
    // parameterization but not change the locus of the surface.
    // Changing m_angle and leaving m_t unchanged, will change the
    // locus of the surface but not change the evaluation domain.
    ON_Interval m_t;

    // If false, the "u" parameter is the angle parameter
    // and the "v" parameter is the curve parameter.
    // If true,  the "u" parameter is the curve parameter
    // and the "v" parameter is the angle parameter.
    ON_BOOL32 m_bTransposed;

    // Bounding box of the surface of revolution.
    ON_BoundingBox m_bbox;

    /*
    Description:
      Use ON_RevSurface::New(...) instead of new ON_RevSurface(...)
    Returns:
      Pointer to an ON_RevSurface.  Destroy by calling delete.
    Remarks:
      See static ON_Brep* ON_Brep::New() for details.
    */
    static ON_RevSurface *New();
    static ON_RevSurface *New(const ON_RevSurface &rev_surface);

    ON_RevSurface();
    ~ON_RevSurface();
    ON_RevSurface(const ON_RevSurface &);
    ON_RevSurface &operator=(const ON_RevSurface &);

    void Destroy();

    ON_BOOL32 SetAngleRadians(double start_angle_radians,
                              double end_angle_radians);

    ON_BOOL32 SetAngleDegrees(double start_angle_degrees,
                              double end_angle_degrees);

    ////////////////////////////////////////////////////////////
    //
    // overrides of virtual ON_Object functions
    //

    // virtual ON_Object::SizeOf override
    unsigned int SizeOf() const;

    // virtual ON_Object::DataCRC override
    ON__UINT32 DataCRC(ON__UINT32 current_remainder) const;

    /*
    Description:
      Tests an object to see if its data members are correctly
      initialized.
    Parameters:
      text_log - [in] if the object is not valid and text_log
          is not NULL, then a brief englis description of the
          reason the object is not valid is appened to the log.
          The information appended to text_log is suitable for
          low-level debugging purposes by programmers and is
          not intended to be useful as a high level user
          interface tool.
    Returns:
      @untitled table
      true     object is valid
      false    object is invalid, uninitialized, etc.
    Remarks:
      Overrides virtual ON_Object::IsValid
    */
    ON_BOOL32 IsValid(ON_TextLog *text_log = NULL) const;

    void Dump(ON_TextLog &) const; // for debugging

    // Use ON_BinaryArchive::WriteObject() and ON_BinaryArchive::ReadObject()
    // for top level serialization.  These Read()/Write() members should just
    // write/read specific definitions.  In particular, they should not write/
    // read any chunk typecode or length information.  The default
    // implementations return false and do nothing.
    ON_BOOL32 Write(ON_BinaryArchive & // serialize definition to binary archive
    ) const;

    ON_BOOL32 Read(ON_BinaryArchive & // restore definition from binary archive
    );

    ////////////////////////////////////////////////////////////
    //
    // overrides of virtual ON_Geometry functions
    //
    int Dimension() const;

    ON_BOOL32 GetBBox(    // returns true if successful
        double *,         // boxmin[dim]
        double *,         // boxmax[dim]
        ON_BOOL32 = false // true means grow box
    ) const;

    void ClearBoundingBox();

    ON_BOOL32 Transform(const ON_Xform &);

    ////////////////////////////////////////////////////////////
    //
    // overrides of virtual ON_Surface functions
    //

    ON_BOOL32 SetDomain(int dir, // 0 sets first parameter's domain, 1 gets
                                 // second parameter's domain
                        double t0, double t1);

    ON_Interval Domain(
        int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const;

    int SpanCount(
        int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const; // number of smooth spans in curve

    ON_BOOL32 GetSpanVector( // span "knots"
        int,     // 0 gets first parameter's domain, 1 gets second parameter's
                 // domain
        double * // array of length SpanCount() + 1
    ) const;     //

    int Degree( // returns maximum algebraic degree of any span
                // ( or a good estimate if curve spans are not algebraic )
        int // 0 gets first parameter's domain, 1 gets second parameter's domain
    ) const;

    ON_BOOL32 GetParameterTolerance( // returns tminus < tplus: parameters
                                     // tminus <= s <= tplus
        int,      // 0 gets first parameter, 1 gets second parameter
        double,   // t = parameter in domain
        double *, // tminus
        double *  // tplus
    ) const;

    /*
    Description:
      Test a surface of revolution to see if it is a portion
      of a sphere.
    Parameters:
      sphere - [out] if not NULL and true is returned,
                    the sphere parameters are filled in.
      tolerance - [in] tolerance to use when checking
    Returns:
      true if the surface of revolution is a portion of a sphere.
    */
    ON_BOOL32 IsSpherical(ON_Sphere *sphere = NULL,
                          double tolerance = ON_ZERO_TOLERANCE) const;

    /*
    Description:
      Test a surface of revolution to see if it is a portion
      of a cylinder.
    Parameters:
      cylinder - [out] if not NULL and true is returned,
                    the cylinder parameters are filled in.
      tolerance - [in] tolerance to use when checking
    Returns:
      true if the surface of revolution is a portion of a cylinder.
    */
    ON_BOOL32 IsCylindrical(ON_Cylinder *cylinder = NULL,
                            double tolerance = ON_ZERO_TOLERANCE) const;

    /*
    Description:
      Test a surface of revolution to see if it is a portion
      of a cone.
    Parameters:
      cone - [out] if not NULL and true is returned,
                    the cone parameters are filled in.
      tolerance - [in] tolerance to use when checking
    Returns:
      true if the surface of revolution is a portion of a cone.
    */
    ON_BOOL32 IsConical(ON_Cone *cone = NULL,
                        double tolerance = ON_ZERO_TOLERANCE) const;

    /*
    Description:
      Test a surface to see if it is planar.
    Parameters:
      plane - [out] if not NULL and true is returned,
                    the plane parameters are filled in.
      tolerance - [in] tolerance to use when checking
    Returns:
      true if there is a plane such that the maximum distance from
      the surface to the plane is <= tolerance.
    Remarks:
      Overrides virtual ON_Surface::IsPlanar.
    */
    ON_BOOL32 IsPlanar(ON_Plane *plane = NULL,
                       double tolerance = ON_ZERO_TOLERANCE) const;

    ON_BOOL32 IsClosed( // true if surface is closed in direction
        int             // dir  0 = "s", 1 = "t"
    ) const;

    ON_BOOL32 IsPeriodic( // true if surface is periodic in direction
        int               // dir  0 = "s", 1 = "t"
    ) const;

    ON_BOOL32 IsSingular( // true if surface side is collapsed to a point
        int               // side of parameter space to test
                          // 0 = south, 1 = east, 2 = north, 3 = west
    ) const;

    /*
    Description:
      Search for a derivatitive, tangent, or curvature
      discontinuity.
    Parameters:
      dir - [in] If 0, then "u" parameter is checked.  If 1, then
                 the "v" parameter is checked.
      c - [in] type of continity to test for.
      t0 - [in] Search begins at t0. If there is a discontinuity
                at t0, it will be ignored.  This makes it
                possible to repeatedly call GetNextDiscontinuity
                and step through the discontinuities.
      t1 - [in] (t0 != t1)  If there is a discontinuity at t1 is
                will be ingored unless c is a locus discontinuity
                type and t1 is at the start or end of the curve.
      t - [out] if a discontinuity is found, then *t reports the
            parameter at the discontinuity.
      hint - [in/out] if GetNextDiscontinuity will be called
         repeatedly, passing a "hint" with initial value *hint=0
         will increase the speed of the search.
      dtype - [out] if not NULL, *dtype reports the kind of
          discontinuity found at *t.  A value of 1 means the first
          derivative or unit tangent was discontinuous.  A value
          of 2 means the second derivative or curvature was
          discontinuous.  A value of 0 means teh curve is not
          closed, a locus discontinuity test was applied, and
          t1 is at the start of end of the curve.
      cos_angle_tolerance - [in] default = cos(1 degree) Used only
          when c is ON::G1_continuous or ON::G2_continuous.  If the
          cosine of the angle between two tangent vectors is
          <= cos_angle_tolerance, then a G1 discontinuity is reported.
      curvature_tolerance - [in] (default = ON_SQRT_EPSILON) Used
          only when c is ON::G2_continuous.  If K0 and K1 are
          curvatures evaluated from above and below and
          |K0 - K1| > curvature_tolerance, then a curvature
          discontinuity is reported.
    Returns:
      Parametric continuity tests c = (C0_continuous, ..., G2_continuous):

        true if a parametric discontinuity was found strictly
        between t0 and t1. Note well that all curves are
        parametrically continuous at the ends of their domains.

      Locus continuity tests c = (C0_locus_continuous, ...,G2_locus_continuous):

        true if a locus discontinuity was found strictly between
        t0 and t1 or at t1 is the at the end of a curve.
        Note well that all open curves (IsClosed()=false) are locus
        discontinuous at the ends of their domains.  All closed
        curves (IsClosed()=true) are at least C0_locus_continuous at
        the ends of their domains.
    */
    bool GetNextDiscontinuity(
        int dir, ON::continuity c, double t0, double t1, double *t,
        int *hint = NULL, int *dtype = NULL,
        double cos_angle_tolerance = ON_DEFAULT_ANGLE_TOLERANCE_COSINE,
        double curvature_tolerance = ON_SQRT_EPSILON) const;

    /*
      Description:
        Test continuity at a surface parameter value.
      Parameters:
        c - [in] continuity to test for
        s - [in] surface parameter to test
        t - [in] surface parameter to test
        hint - [in] evaluation hint
        point_tolerance - [in] if the distance between two points is
            greater than point_tolerance, then the surface is not C0.
        d1_tolerance - [in] if the difference between two first derivatives is
            greater than d1_tolerance, then the surface is not C1.
        d2_tolerance - [in] if the difference between two second derivatives is
            greater than d2_tolerance, then the surface is not C2.
        cos_angle_tolerance - [in] default = cos(1 degree) Used only when
            c is ON::G1_continuous or ON::G2_continuous.  If the cosine
            of the angle between two normal vectors
            is <= cos_angle_tolerance, then a G1 discontinuity is reported.
        curvature_tolerance - [in] (default = ON_SQRT_EPSILON) Used only when
            c is ON::G2_continuous.  If K0 and K1 are curvatures evaluated
            from above and below and |K0 - K1| > curvature_tolerance,
            then a curvature discontinuity is reported.
      Returns:
        true if the surface has at least the c type continuity at the parameter
      t. Remarks: Overrides virtual ON_Surface::IsContinuous
      */
    bool
    IsContinuous(ON::continuity c, double s, double t, int *hint = NULL,
                 double point_tolerance = ON_ZERO_TOLERANCE,
                 double d1_tolerance = ON_ZERO_TOLERANCE,
                 double d2_tolerance = ON_ZERO_TOLERANCE,
                 double cos_angle_tolerance = ON_DEFAULT_ANGLE_TOLERANCE_COSINE,
                 double curvature_tolerance = ON_SQRT_EPSILON) const;

    ON_BOOL32
    Reverse( // reverse parameterizatrion, Domain changes from [a,b] to [-b,-a]
        int  // dir  0 = "s", 1 = "t"
    );

    ON_BOOL32
    Transpose(); // transpose surface parameterization (swap "s" and "t")

    ON_BOOL32 Evaluate( // returns false if unable to evaluate
        double, double, // evaluation parameters  (see m_bTransposed)
        int,            // number of derivatives (>=0)
        int,            // array stride (>=Dimension())
        double *,       // array of length stride*(ndir+1)*(ndir+2)/2
        int = 0,        // optional - determines which quadrant to evaluate from
                        //         0 = default
                        //         1 from NE quadrant
                        //         2 from NW quadrant
                        //         3 from SW quadrant
                        //         4 from SE quadrant
        int * = 0       // optional - evaluation hint (int[2]) used to speed
                        //            repeated evaluations
    ) const;

    /*
    Description:
      Get isoparametric curve.
      Overrides virtual ON_Surface::IsoCurve.
    Parameters:
      dir - [in] 0 first parameter varies and second parameter is constant
                   e.g., point on IsoCurve(0,c) at t is srf(t,c)
                 1 first parameter is constant and second parameter varies
                   e.g., point on IsoCurve(1,c) at t is srf(c,t)

      c - [in] value of constant parameter
    Returns:
      Isoparametric curve.
    */
    ON_Curve *IsoCurve(int dir, double c) const;

    /*
    Description:
      Removes the portions of the surface outside of the specified interval.

    Parameters:
      dir - [in] 0  The domain specifies an sub-interval of Domain(0)
                    (the first surface parameter).
                 1  The domain specifies an sub-interval of Domain(1)
                    (the second surface parameter).
      domain - [in] interval of the surface to keep. If dir is 0, then
          the portions of the surface with parameters (s,t) satisfying
          s < Domain(0).Min() or s > Domain(0).Max() are trimmed away.
          If dir is 1, then the portions of the surface with parameters
          (s,t) satisfying t < Domain(1).Min() or t > Domain(1).Max()
          are trimmed away.
    */
    ON_BOOL32 Trim(int dir, const ON_Interval &domain);

    /*
     Description:
       Where possible, analytically extends surface to include domain.
     Parameters:
       dir - [in] 0  new Domain(0) will include domain.
                     (the first surface parameter).
                  1  new Domain(1) will include domain.
                     (the second surface parameter).
       domain - [in] if domain is not included in surface domain,
       surface will be extended so that its domain includes domain.
       Will not work if surface is closed in direction dir.
       Original surface is identical to the restriction of the
       resulting surface to the original surface domain,
     Returns:
       true if successful.
       */
    bool Extend(int dir, const ON_Interval &domain);

    /*
    Description:
      Splits (divides) the surface into two parts at the
      specified parameter.

    Parameters:
      dir - [in] 0  The surface is split vertically.  The "west" side
                    is returned in "west_or_south_side" and the "east"
                    side is returned in "east_or_north_side".
                 1  The surface is split horizontally.  The "south" side
                    is returned in "west_or_south_side" and the "north"
                    side is returned in "east_or_north_side".
      c - [in] value of constant parameter in interval returned
                 by Domain(dir)
      west_or_south_side - [out] west/south portion of surface returned here
      east_or_north_side - [out] east/north portion of surface returned here

    Example:

            ON_NurbsSurface srf = ...;
            int dir = 1;
            ON_RevSurface* south_side = 0;
            ON_RevSurface* north_side = 0;
            srf.Split( dir, srf.Domain(dir).Mid() south_side, north_side );

    */
    ON_BOOL32 Split(int dir, double c, ON_Surface *&west_or_south_side,
                    ON_Surface *&east_or_north_side) const;

    int
    GetNurbForm( // returns 0: unable to create NURBS representation
                 //            with desired accuracy.
                 //         1: success - returned NURBS parameterization
                 //            matches the surface's to wthe desired accuracy
                 //         2: success - returned NURBS point locus matches
                 //            the surfaces's to the desired accuracy but, on
                 //            the interior of the surface's domain, the
                 //            surface's parameterization and the NURBS
                 //            parameterization may not match to the
                 //            desired accuracy.
        ON_NurbsSurface &, double = 0.0) const;

    int HasNurbForm( // returns 0: unable to create NURBS representation
                     //            with desired accuracy.
                     //         1: success - returned NURBS parameterization
                     //            matches the surface's to wthe desired
                     //            accuracy
                     //         2: success - returned NURBS point locus matches
                     //            the surfaces's to the desired accuracy but,
                     //            on the interior of the surface's domain, the
                     //            surface's parameterization and the NURBS
                     //            parameterization may not match to the
                     //            desired accuracy.
    ) const;

    bool GetSurfaceParameterFromNurbFormParameter(double nurbs_s,
                                                  double nurbs_t,
                                                  double *surface_s,
                                                  double *surface_t) const;

    bool GetNurbFormParameterFromSurfaceParameter(double surface_s,
                                                  double surface_t,
                                                  double *nurbs_s,
                                                  double *nurbs_t) const;
};

#endif
