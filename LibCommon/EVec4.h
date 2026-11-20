// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_VEC_4_H
#define E_VEC_4_H

#include <DefineLibCommon.h>
#include <EVec3.h>

/*A vector that has 4 values, you can use this struct as Quat or color.*/
struct  COMMON_DLL EVec4 {
    EVec4();
    EVec4(double v1, double v2, double v3, double v4);
    virtual ~EVec4();
    union {
        double v[4];
        struct {
            double x;
            double y;
            double z;
            double w;
        };
        struct {
            double r;
            double g;
            double b;
            double a;
        };
    };
    EVec4& operator=(const EVec4& value);
    /// Binary multiply
    EVec4 operator*(const EVec4& rhs) ;

    double length() const;

    /// Length of the quaternion = vec . vec
    double length2() const;
    //Set the elements of the Quat to represent a rotation of angle, (radians) around the axis (x,y,z)
    void makeRotate( double angle, double x, double y, double z );
    void makeRotate( double angle, const EVec3& axis);

    void makeRotate( double angle1, const EVec3& axis1,
            double angle2, const EVec3& axis2,
            double angle3, const EVec3& axis3);

    EVec3 getEulerAngle();
};

struct COMMON_DLL EColor : public EVec4
{
    EColor();
    EColor(double r, double g, double b, double a);
    EColor& operator=(const EColor& value);
};

#endif // E_VEC_4_H
