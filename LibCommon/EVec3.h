// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef E_VEC3_H
#define E_VEC3_H

#include <DefineLibCommon.h>

struct COMMON_DLL EVec3 {
    EVec3();
    EVec3(double v1, double v2, double v3);
    virtual ~EVec3();
    union {
        double v[3];
        struct {
            double x;
            double y;
            double z;
        };
    };

    EVec3& operator=(const EVec3& value);
    /** Binary vector subtract. */
    EVec3 operator - (const EVec3& rhs);
    /** Multiply by scalar. */
    EVec3 operator * (double rhs);
    EVec3 operator *= (double rhs);
    /** Binary vector add. */
    EVec3 operator + (const EVec3& rhs);
    EVec3& operator += (const EVec3& rhs);

    void normalize();
    double length();


    static void Cross(const double* a, const double* b, double* r);
    static double Dot(const double* a, const double* b);
    static void Normalize(const double* a, double* r);
};


#endif //E_VEC3_H
