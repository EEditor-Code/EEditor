// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_VARIANT_H
#define E_VARIANT_H

#include <DefineLibCommon.h>
#include <EMatrix4x4.h>
#include <EVec2.h>
#include <EString.h>
#include <SmartPtr/ESmartPtr.h>
#include <cstdint>
#include <vector>

using EIntList    = std::vector<int>;
using EDoubleList = std::vector<double>;
using EStringList = std::vector<EString>;

class COMMON_DLL EVariant
{
public:
    enum EVariantType
    {
        INT,
        DOUBLE,
        BOOL,
        LONG,
        VEC2,
        VEC3,
        VEC4,
        COLOR,
        MATRIX_4X4,
        STRING,
        INTPTR_T,
        EMPTY,
        VARIANT,
        INT_LIST,
        DOUBLE_LIST,
        STRING_LIST
    };
    enum AsVariantTag { AsVariant };

    EVariant();
    EVariant(int value);
    EVariant(long value);
    EVariant(double value);
    EVariant(bool value);
    EVariant(const EVec2& value);
    EVariant(const EVec3& value);
    EVariant(const EVec4& value);
    EVariant(const EColor& value);
    EVariant(const EMatrix4x4& value);
    EVariant(const EString& value);
    EVariant(const char* value);
    EVariant(const EIntList& value);
    EVariant(const EDoubleList& value);
    EVariant(const EStringList& value);
    
#ifdef _WIN32
    EVariant(intptr_t value);
#endif

    EVariant(const EVariant& value);
    EVariant(const EVariant& value, AsVariantTag);
    
    ~EVariant();

    EVariantType m_type;

    union 
    {
        int value_int;
        double value_double;
        bool value_bool;
        intptr_t value_ptr;
        long value_long;
    };

    //union
    //{
    //    EVec3* value_EVec3;
    //    EVec4* value_EVec4;
    //    EMatrix4x4* value_EMatrix4x4;
    //    EString* value_EString;
    //    intptr_t value_ptr;
    //};

    void clear();

    EVariant& operator=(int value);
    EVariant& operator=(long value);
    EVariant& operator=(double value);
    EVariant& operator=(bool value);
    EVariant& operator=(const EVec2& value);
    EVariant& operator=(const EVec3& value);
    EVariant& operator=(const EVec4& value);
    EVariant& operator=(const EColor& value);
    EVariant& operator=(const EMatrix4x4& value);
    EVariant& operator=(const char* value);
    EVariant& operator=(const EString& value);
    EVariant& operator=(const EIntList& value);
    EVariant& operator=(const EDoubleList& value);
    EVariant& operator=(const EStringList& value);
#ifdef _WIN32
    EVariant& operator=(intptr_t value);
#endif
    EVariant& operator=(const EVariant& value);

    const int& to_int() const;
    int& to_int();

    const long& to_long() const;
    long& to_long();

    const double& to_double() const;
    double& to_double();

    const bool& to_bool() const;
    bool& to_bool();

    const EVec2& to_EVec2() const;
    EVec2& to_EVec2();

    const EVec3& to_EVec3() const;
    EVec3& to_EVec3();

    const EVec4& to_EVec4() const;
    EVec4& to_EVec4();

    const EColor& to_EColor() const;
    EColor& to_EColor();

    const EMatrix4x4& to_EMatrix4x4() const;
    EMatrix4x4& to_EMatrix4x4();

    const EString& to_EString() const;
    EString& to_EString();

    intptr_t to_intptr_t() const;

    const EVariant& to_EVariant() const;
    EVariant& to_EVariant();

    const EIntList& to_EIntList() const;
    EIntList& to_EIntList();

    const EDoubleList& to_EDoubleList() const;
    EDoubleList& to_EDoubleList();

    const EStringList& to_EStringList() const;
    EStringList& to_EStringList();

    /*just for debug*/
    EString getStrForDebug();

    EString getStrForDisplay();
};

#endif //E_VARIANT_H 
