// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EVec2.h"
#include <cmath>
#include <cfloat>


EVec2::EVec2()
{
    x = 0;
    y = 0;
}

EVec2::EVec2(double v1, double v2)
{
    x = v1;
    y = v2;
}

EVec2::~EVec2()
{
}
    
EVec2& EVec2::operator=(const EVec2& value)
{
    x = value.x;
    y = value.y;

    return *this;
}

EVec2 EVec2::operator - (const EVec2& rhs)
{
    return EVec2(v[0]-rhs.v[0], 
                 v[1]-rhs.v[1] 
                 );
}

EVec2 EVec2::operator * (double rhs)
{
    return EVec2(v[0]*rhs, v[1]*rhs);
}

EVec2 EVec2::operator *= (double rhs)
{
    v[0] *= rhs;
    v[1] *= rhs;
    return *this;
}

EVec2 EVec2::operator + (const EVec2& rhs)
{
    return EVec2(v[0]+rhs.v[0], v[1]+rhs.v[1]);
}

EVec2& EVec2::operator += (const EVec2& rhs)
{
    v[0] += rhs.v[0];
    v[1] += rhs.v[1];
    return *this;
}

