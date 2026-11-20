// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 北京格瑞特皮软件科技有限公司

#ifndef E_MATRIX_4X4_H
#define E_MATRIX_4X4_H

#include <DefineLibCommon.h>
#include <EVec4.h>
#include <EVec3.h>


struct  COMMON_DLL EMatrix4x4
{
    /**参考系: 同一份矩阵数据按"局部"还是"世界"解读。
     *
     * ⚠️ 它**不影响矩阵数值** —— 16 个数怎么算还是怎么算; frame 只改变**语义与显示**:
     *   - LOCAL: data 就是相对父节点的局部矩阵(直接读写 vsg::MatrixTransform::matrix)
     *   - WORLD: data 是场景根下的世界矩阵(根节点类在 OnGet/OnSet 里做一次
     *            "父世界 × 局部" / "逆(父世界) × 世界" 的折算)
     *
     * ⚠️ 它**不是节点数据** —— vsg::MatrixTransform 只有 16 个数, 存不下 frame。
     *    所以每个 OnGet 都必须在最后把传入的 frame **原样写回**(整体赋值会冲掉它),
     *    否则每次读属性 frame 都会被重置。*/
    enum Frame
    {
        LOCAL = 0,
        WORLD = 1
    };

    EMatrix4x4();
    virtual ~EMatrix4x4();

    union {
        double data[4][4];
        double dataVec[16];
    };

    /**参考系(默认局部 = 与加本字段之前的行为一致)*/
    Frame frame = LOCAL;

    /**⚠️ 本函数**只拷 16 个数**(历史实现), 加 frame 字段后必须同步拷贝它 ——
     *  否则经过 operator= 的地方(如 EVariant 的赋值路径)会静默丢掉 frame。*/
    EMatrix4x4& operator=(const EMatrix4x4& value);

    void makeIdentity();

    /**4×4 相乘 (this * rhs)。
     * ⚠️ 本类用**行向量**约定(平移在 data[3][0..2], 见 makeTranslate), 故
     *    子→父的复合是 `子 * 父`, 与 vsg::dmat4 的列向量约定**相乘顺序相反**。*/
    EMatrix4x4 operator*(const EMatrix4x4& rhs) const;

    /**逆矩阵(高斯-约当, 支持任意可逆矩阵)。
     * @param ok 可空; 不可逆时返回单位阵并置 *ok=false。*/
    EMatrix4x4 inverse(bool* ok = nullptr) const;
    void makeTranslate(double x, double y, double z);
    void makeTranslate(const EVec3& trans);
    
    void setTranslate(double x, double y, double z);
    void setTranslate(const EVec3& trans);

    void setRotate(const EVec4& quat);

    void makeScale(double x, double y, double z);
    void makeScale(const EVec3& scale);
    
    void makeRotate( double angle, double x, double y, double z );
    void makeRotate( double angle1, const EVec3& axis1,
            double angle2, const EVec3& axis2,
            double angle3, const EVec3& axis3);
    void makeRotate(const EVec4& quat);

    inline EVec3 getTrans() const { return EVec3(data[3][0], data[3][1], data[3][2]); }
    EVec4 getQuat();
    //get rotate as euler angle, default is x-y-z sequence
    EVec3 getEulerAngle();

    //same as opengl's LookAt function
   void makeLookAt(const EVec3& eye, const EVec3& at, const EVec3& up);

   static void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16);
   static void Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16);

};

#endif // E_MATRIX_4X4_H
