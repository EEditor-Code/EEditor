// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#include "EMatrix4x4.h"
#include <cmath>
#include <utility>
#include <limits>

#define SET_ROW(row, v1, v2, v3, v4 )    \
    data[(row)][0] = (v1); \
    data[(row)][1] = (v2); \
    data[(row)][2] = (v3); \
    data[(row)][3] = (v4);

EMatrix4x4::EMatrix4x4()
{
    this->makeIdentity();
}

EMatrix4x4::~EMatrix4x4()
{
}

EMatrix4x4& EMatrix4x4::operator=(const EMatrix4x4& value)
{
    for(int i = 0 ; i < 4; i++)
    {
        for(int j = 0 ; j< 4; j++)
        {
            data[i][j] = value.data[i][j];
        }
    }

    // ⚠️ 必须同步: 本函数历史上只拷 16 个数, 加 frame 字段后漏掉这行会**静默丢参考系**
    // (EVariant 的赋值路径走的就是这里)
    frame = value.frame;

    return *this;
}

void EMatrix4x4::makeIdentity()
{
    SET_ROW(0,    1, 0, 0, 0 )
    SET_ROW(1,    0, 1, 0, 0 )
    SET_ROW(2,    0, 0, 1, 0 )
    SET_ROW(3,    0, 0, 0, 1 )
}

void EMatrix4x4::makeTranslate(double x, double y, double z)
{
    SET_ROW(0,    1, 0, 0, 0 )
    SET_ROW(1,    0, 1, 0, 0 )
    SET_ROW(2,    0, 0, 1, 0 )
    SET_ROW(3,    x, y, z, 1 )
}

void EMatrix4x4::makeTranslate(const EVec3& trans)
{
    this->makeTranslate(trans.x, trans.y, trans.z);
}

void EMatrix4x4::setTranslate(double x, double y, double z)
{
    SET_ROW(3,    x, y, z, 1 )
}

void EMatrix4x4::setTranslate(const EVec3& trans)
{
    this->setTranslate(trans.x, trans.y, trans.z);
}

void EMatrix4x4::setRotate(const EVec4& quat)
{
    EVec3 trans = this->getTrans();
    this->makeRotate(quat);
    this->setTranslate(trans);
}

void EMatrix4x4::makeScale(double x, double y, double z)
{
    SET_ROW(0,    x, 0, 0, 0 )
    SET_ROW(1,    0, y, 0, 0 )
    SET_ROW(2,    0, 0, z, 0 )
    SET_ROW(3,    0, 0, 0, 1 )
}

void EMatrix4x4::makeScale(const EVec3& scale)
{
    this->makeTranslate(scale.x, scale.y, scale.z);
}

void EMatrix4x4::makeRotate( double angle, double x, double y, double z )
{
    makeIdentity();

    EVec4 quat;
    quat.makeRotate( angle, x, y, z);
    makeRotate(quat);
}

void EMatrix4x4::makeRotate( double angle1, const EVec3& axis1,
            double angle2, const EVec3& axis2,
            double angle3, const EVec3& axis3)
{
}

void EMatrix4x4::makeRotate(const EVec4& q)
{
    double length2 = q.length2();
    if (fabs(length2) <= std::numeric_limits<double>::min())
    {
        data[0][0] = 0.0; data[1][0] = 0.0; data[2][0] = 0.0;
        data[0][1] = 0.0; data[1][1] = 0.0; data[2][1] = 0.0;
        data[0][2] = 0.0; data[1][2] = 0.0; data[2][2] = 0.0;
    }
    else
    {
        double rlength2;
        // normalize quat if required.
        // We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
        // That is a square of a square root, so it is possible to avoid that
        if (length2 != 1.0)
        {
            rlength2 = 2.0/length2;
        }
        else
        {
            rlength2 = 2.0;
        }

        // Source: Gamasutra, Rotating Objects Using Quaternions
        //
        //http://www.gamasutra.com/features/19980703/quaternions_01.htm

        double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

        // calculate coefficients
        x2 = rlength2*q.x;
        y2 = rlength2*q.y;
        z2 = rlength2*q.z;

        xx = q.x * x2;
        xy = q.x * y2;
        xz = q.x * z2;

        yy = q.y * y2;
        yz = q.y * z2;
        zz = q.z * z2;

        wx = q.w * x2;
        wy = q.w * y2;
        wz = q.w * z2;

        // Note.  Gamasutra gets the matrix assignments inverted, resulting
        // in left-handed rotations, which is contrary to OpenGL and OSG's
        // methodology.  The matrix assignment has been altered in the next
        // few lines of code to do the right thing.
        // Don Burns - Oct 13, 2001
        data[0][0] = 1.0 - (yy + zz);
        data[1][0] = xy - wz;
        data[2][0] = xz + wy;


        data[0][1] = xy + wz;
        data[1][1] = 1.0 - (xx + zz);
        data[2][1] = yz - wx;

        data[0][2] = xz - wy;
        data[1][2] = yz + wx;
        data[2][2] = 1.0 - (xx + yy);
    }

}

EVec4 EMatrix4x4::getQuat()
{
    EVec4 q;

    double QW, QX, QY, QZ;
    double s;
    double tq[4];
    int i, j;

    // Use tq to store the largest trace
    tq[0] = 1 + data[0][0]+data[1][1]+data[2][2];
    tq[1] = 1 + data[0][0]-data[1][1]-data[2][2];
    tq[2] = 1 - data[0][0]+data[1][1]-data[2][2];
    tq[3] = 1 - data[0][0]-data[1][1]+data[2][2];

    // Find the maximum (could also use stacked if's later)
    j = 0;
    for(i=1;i<4;i++) j = (tq[i]>tq[j])? i : j;

    // check the diagonal
    if (j==0)
    {
        /* perform instant calculation */
        QW = tq[0];
        QX = data[1][2]-data[2][1];
        QY = data[2][0]-data[0][2];
        QZ = data[0][1]-data[1][0];
    }
    else if (j==1)
    {
        QW = data[1][2]-data[2][1];
        QX = tq[1];
        QY = data[0][1]+data[1][0];
        QZ = data[2][0]+data[0][2];
    }
    else if (j==2)
    {
        QW = data[2][0]-data[0][2];
        QX = data[0][1]+data[1][0];
        QY = tq[2];
        QZ = data[1][2]+data[2][1];
    }
    else /* if (j==3) */
    {
        QW = data[0][1]-data[1][0];
        QX = data[2][0]+data[0][2];
        QY = data[1][2]+data[2][1];
        QZ = tq[3];
    }

    s = std::sqrt(0.25/tq[j]);
    QW *= s;
    QX *= s;
    QY *= s;
    QZ *= s;

    q.w = QW;
    q.x = QX;
    q.y = QY;
    q.z = QZ;

    return q;
}

EVec3 EMatrix4x4::getEulerAngle()
{
    double sy = sqrt(
            this->data[0][0] * this->data[0][0]+this->data[0][1] * this->data[0][1]);
    bool singular = sy < 1e-6;
    double x, y, z;
    if (!singular)
    {
        x = atan2( this->data[1][2], this->data[2][2]);
        y = atan2(-this->data[0][2], sy);
        z = atan2( this->data[0][1], this->data[0][0]);
    }
    else
    {
        x = atan2(-this->data[2][1], this->data[1][1]);
        y = atan2(-this->data[0][2], sy);
        z = 0;
    }

    return EVec3(x,y,z);
}

void EMatrix4x4::makeLookAt(const EVec3& eye, const EVec3& at, const EVec3& up)
{
    double X[3], Y[3], Z[3], tmp[3];

    tmp[0] = eye.v[0] - at.v[0];
    tmp[1] = eye.v[1] - at.v[1];
    tmp[2] = eye.v[2] - at.v[2];

    EVec3::Normalize(tmp, Z);
    EVec3::Normalize(up.v, Y);
    EVec3::Cross(Y, Z, tmp);
    EVec3::Normalize(tmp, X);
    EVec3::Cross(Z, X, tmp);
    EVec3::Normalize(tmp, Y);

      this->dataVec[0] = X[0];
      this->dataVec[1] = Y[0];
      this->dataVec[2] = Z[0];
      this->dataVec[3] = 0.0f;
      this->dataVec[4] = X[1];
      this->dataVec[5] = Y[1];
      this->dataVec[6] = Z[1];
      this->dataVec[7] = 0.0f;
      this->dataVec[8] = X[2];
      this->dataVec[9] = Y[2];
      this->dataVec[10] = Z[2];
      this->dataVec[11] = 0.0f;
      this->dataVec[12] = -EVec3::Dot(X, eye.v);
      this->dataVec[13] = -EVec3::Dot(Y, eye.v);
      this->dataVec[14] = -EVec3::Dot(Z, eye.v);
      this->dataVec[15] = 1.0f;
}

void EMatrix4x4::Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16)
{
   float ymax, xmax;
   ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
   xmax = ymax * aspectRatio;
   Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

void EMatrix4x4::Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16)
{
    float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    m16[0] = temp / temp2;
    m16[1] = 0.0;
    m16[2] = 0.0;
    m16[3] = 0.0;
    m16[4] = 0.0;
    m16[5] = temp / temp3;
    m16[6] = 0.0;
    m16[7] = 0.0;
    m16[8] = (right + left) / temp2;
    m16[9] = (top + bottom) / temp3;
    m16[10] = (-zfar - znear) / temp4;
    m16[11] = -1.0f;
    m16[12] = 0.0;
    m16[13] = 0.0;
    m16[14] = (-temp * zfar) / temp4;
    m16[15] = 0.0;
}

/* ===== 4×4 乘法与求逆(2026-10-19; 换父保持世界位姿用, 见 EItem::onReparentKeepWorld) ===== */
EMatrix4x4 EMatrix4x4::operator*(const EMatrix4x4& rhs) const
{
    EMatrix4x4 r;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            double s = 0.0;
            for (int k = 0; k < 4; ++k) s += data[i][k] * rhs.data[k][j];
            r.data[i][j] = s;
        }
    }
    r.frame = frame;   // 结果参考系沿用左操作数(与 operator= 一样不能丢 frame)
    return r;
}

EMatrix4x4 EMatrix4x4::inverse(bool* ok) const
{
    // 高斯-约当: [A | I] → [I | A⁻¹]
    double a[4][8];
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j) a[i][j] = data[i][j];
        for (int j = 0; j < 4; ++j) a[i][4 + j] = (i == j) ? 1.0 : 0.0;
    }

    bool invertible = true;
    for (int col = 0; col < 4; ++col)
    {
        int piv = col;                          // 部分主元
        for (int r = col + 1; r < 4; ++r)
        {
            if (std::fabs(a[r][col]) > std::fabs(a[piv][col])) piv = r;
        }
        if (std::fabs(a[piv][col]) < 1e-12)     // 退化: 返回单位阵
        {
            invertible = false;
            break;
        }
        if (piv != col)
        {
            for (int j = 0; j < 8; ++j) std::swap(a[col][j], a[piv][j]);
        }

        const double d = a[col][col];
        for (int j = 0; j < 8; ++j) a[col][j] /= d;
        for (int r = 0; r < 4; ++r)
        {
            if (r == col) continue;
            const double f = a[r][col];
            if (f == 0.0) continue;
            for (int j = 0; j < 8; ++j) a[r][j] -= f * a[col][j];
        }
    }

    EMatrix4x4 r;
    r.makeIdentity();
    if (invertible)
    {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) r.data[i][j] = a[i][4 + j];
    }
    if (ok) *ok = invertible;
    return r;
}
