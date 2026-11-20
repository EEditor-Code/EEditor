// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#include "EVec3.h"
#include <cmath>
#include <cfloat>


EVec3::EVec3()
{
    x = 0;
    y = 0;
    z = 0;
}

EVec3::EVec3(double v1, double v2, double v3)
{
    x = v1;
    y = v2;
    z = v3;
}

EVec3::~EVec3()
{
}
    
EVec3& EVec3::operator=(const EVec3& value)
{
    x = value.x;
    y = value.y;
    z = value.z;

    return *this;
}

EVec3 EVec3::operator - (const EVec3& rhs)
{
    return EVec3(v[0]-rhs.v[0], 
                 v[1]-rhs.v[1], 
                 v[2]-rhs.v[2]);
}

EVec3 EVec3::operator * (double rhs)
{
    return EVec3(v[0]*rhs, v[1]*rhs, v[2]*rhs);
}

EVec3 EVec3::operator *= (double rhs)
{
    v[0] *= rhs;
    v[1] *= rhs;
    v[2] *= rhs;
    return *this;
}

EVec3 EVec3::operator + (const EVec3& rhs)
{
    return EVec3(v[0]+rhs.v[0], v[1]+rhs.v[1], v[2]+rhs.v[2]);
}

EVec3& EVec3::operator += (const EVec3& rhs)
{
    v[0] += rhs.v[0];
    v[1] += rhs.v[1];
    v[2] += rhs.v[2];
    return *this;
}

void EVec3::normalize()
{
    double il = 1.f / (std::sqrt(Dot(v, v)) + DBL_EPSILON);
    v[0] = v[0] * il;
    v[1] = v[1] * il;
    v[2] = v[2] * il;

}

double EVec3::length()
{
    double length = std::sqrt(Dot(v, v));
    return length;
}

void EVec3::Cross(const double* a, const double* b, double* r)
{
    r[0] = a[1] * b[2] - a[2] * b[1];
    r[1] = a[2] * b[0] - a[0] * b[2];
    r[2] = a[0] * b[1] - a[1] * b[0];
}

double EVec3::Dot(const double* a, const double* b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void EVec3::Normalize(const double* a, double* r)
{
    double il = 1.f / (std::sqrt(Dot(a, a)) + DBL_EPSILON);
    r[0] = a[0] * il;
    r[1] = a[1] * il;
    r[2] = a[2] * il;
}
