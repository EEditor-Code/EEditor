// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EVec4.h"
#include <cmath>


EVec4::EVec4()
{
    x = 1;
    y = 0;
    z = 0;
    w = 0;
}

EVec4::EVec4(double v1, double v2, double v3, double v4)
{
    x = v1;
    y = v2;
    z = v3;
    w = v4;
}

EVec4::~EVec4()
{
}

EVec4& EVec4::operator=(const EVec4& value)
{
    this->x = value.x;
    this->y = value.y;
    this->z = value.z;
    this->w = value.w;

    return *this;
}

EVec4 EVec4::operator*(const EVec4& rhs)
{
    return EVec4
        ( rhs.v[3]*v[0] + rhs.v[0]*v[3] + rhs.v[1]*v[2] - rhs.v[2]*v[1],
          rhs.v[3]*v[1] - rhs.v[0]*v[2] + rhs.v[1]*v[3] + rhs.v[2]*v[0],
          rhs.v[3]*v[2] + rhs.v[0]*v[1] - rhs.v[1]*v[0] + rhs.v[2]*v[3],
          rhs.v[3]*v[3] - rhs.v[0]*v[0] - rhs.v[1]*v[1] - rhs.v[2]*v[2] );
}

double EVec4::length() const
{
    return std::sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

/// Length of the quaternion = vec . vec
double EVec4::length2() const
{
    return v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3];
}

void EVec4::makeRotate( double angle, double x, double y, double z )
{
    const double epsilon = 0.0000001;

    double length = std::sqrt( x*x + y*y + z*z );
    if (length < epsilon)
    {
        // ~zero length axis, so reset rotation to zero.
        this->x = 1;
        this->y = 0;
        this->z = 0;
        this->w = 0;
        return;
    }

    double inversenorm  = 1.0/length;
    double coshalfangle = cos( 0.5*angle );
    double sinhalfangle = sin( 0.5*angle );

    this->x = x * sinhalfangle * inversenorm;
    this->y = y * sinhalfangle * inversenorm;
    this->z = z * sinhalfangle * inversenorm;
    this->w = coshalfangle;
}

void EVec4::makeRotate( double angle, const EVec3& axis)
{
    this->makeRotate(angle, axis.x, axis.y, axis.z);
}

void EVec4::makeRotate( double angle1, const EVec3& axis1,
        double angle2, const EVec3& axis2,
        double angle3, const EVec3& axis3)
{
    EVec4 q1; q1.makeRotate(angle1,axis1);
    EVec4 q2; q2.makeRotate(angle2,axis2);
    EVec4 q3; q3.makeRotate(angle3,axis3);

    *this = q1*q2*q3;
}

EVec3 EVec4::getEulerAngle()
{
    return EVec3();
}

EColor::EColor()
{
    this->r = 0;
    this->g = 0;
    this->b = 0;
    this->a = 1;
}

EColor::EColor(double r, double g, double b, double a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

EColor& EColor::operator=(const EColor& value)
{
    this->x = value.x;
    this->y = value.y;
    this->z = value.z;
    this->w = value.w;

    return *this;
}
