// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef E_VEC2_H
#define E_VEC2_H

#include <DefineLibCommon.h>

struct COMMON_DLL EVec2 {
    EVec2();
    EVec2(double v1, double v2);
    virtual ~EVec2();
    union {
        double v[2];
        struct {
            double x;
            double y;
        };
    };

    EVec2& operator=(const EVec2& value);
    /** Binary vector subtract. */
    EVec2 operator - (const EVec2& rhs);
    /** Multiply by scalar. */
    EVec2 operator * (double rhs);
    EVec2 operator *= (double rhs);
    /** Binary vector add. */
    EVec2 operator + (const EVec2& rhs);
    EVec2& operator += (const EVec2& rhs);

};


#endif //E_VEC2_H
