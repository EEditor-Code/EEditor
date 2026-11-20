// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EVariant.h"
#include "EMatrix4x4.h"
#include "EString.h"
#include <locale>

EVariant::EVariant()
{
    this->m_type = EVariant::EVariantType::EMPTY;
    value_ptr = NULL;
    this->value_int = 0;
}

EVariant::EVariant(int value)
{
    this->m_type = EVariant::EVariantType::INT;
    value_int = value;
}

EVariant::EVariant(long value)
{
   this->m_type = EVariant::EVariantType::LONG;
   value_long = value;
}

EVariant::EVariant(double value)
{
    this->m_type = EVariant::EVariantType::DOUBLE;
    value_double = value;
}

EVariant::EVariant(bool value)
{
    this->m_type = EVariant::EVariantType::BOOL;
    value_bool = value;
}

// EVariant::EVariant(long value)
// {
    // this->m_type = EVariant::EVariantType::LONG;
    // value_long = value;
// }

EVariant::EVariant(const EVec2& value)
{
    this->m_type = EVariant::EVariantType::VEC2;
    EVec2* tempPtr = new EVec2;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EVec3& value)
{
    this->m_type = EVariant::EVariantType::VEC3;
    EVec3* tempPtr = new EVec3;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EVec4& value)
{
    this->m_type = EVariant::EVariantType::VEC4;
    EVec4* tempPtr = new EVec4;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EColor& value)
{
    this->m_type = EVariant::EVariantType::COLOR;
    EColor* tempPtr = new EColor;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EMatrix4x4& value)
{
    this->m_type = EVariant::EVariantType::MATRIX_4X4;
    EMatrix4x4* tempPtr = new EMatrix4x4;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EString& value)
{
    this->m_type = EVariant::EVariantType::STRING;
    EString* tempPtr = new EString;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}
EVariant::EVariant(const char* value)
{
    this->m_type = EVariant::EVariantType::STRING;
    EString* tempPtr = new EString;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EIntList& value)
{
    this->m_type = EVariant::EVariantType::INT_LIST;
    auto* tempPtr = new EIntList;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EDoubleList& value)
{
    this->m_type = EVariant::EVariantType::DOUBLE_LIST;
    auto* tempPtr = new EDoubleList;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

EVariant::EVariant(const EStringList& value)
{
    this->m_type = EVariant::EVariantType::STRING_LIST;
    auto* tempPtr = new EStringList;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
}

#ifdef _WIN32
EVariant::EVariant(intptr_t value)
{
    this->m_type = EVariant::EVariantType::INTPTR_T;
    value_ptr = value;
}
#endif

EVariant::EVariant(const EVariant& value)
{
    this->m_type = EVariant::EVariantType::EMPTY;
    value_ptr = NULL;
    this->value_int = 0;

    *this = value;
}

EVariant::EVariant(const EVariant& value, AsVariantTag)
{
    this->m_type = EVariant::EVariantType::EMPTY;
    value_ptr = NULL;
    this->value_int = 0;

    EVariant* tempPtr = new EVariant;
    *tempPtr = value;
    value_ptr = (intptr_t)tempPtr;
    m_type = EVariant::EVariantType::VARIANT;
}

EVariant::~EVariant()
{
    this->clear();
}

void EVariant::clear()
{
    if(m_type > EVariant::EVariantType::BOOL)
    {
        if(m_type == EVariant::EVariantType::VEC2)
        {
            EVec2* valuePtr = (EVec2*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::VEC3)
        {
            EVec3* valuePtr = (EVec3*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::VEC4)
        {
            EVec4* valuePtr = (EVec4*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::COLOR)
        {
            EColor* valuePtr = (EColor*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::MATRIX_4X4)
        {
            EMatrix4x4* valuePtr = (EMatrix4x4*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::STRING)
        {
            EString* valuePtr = (EString*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::VARIANT)
        {
            EVariant* valuePtr = (EVariant*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::INT_LIST)
        {
            auto* valuePtr = (EIntList*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::DOUBLE_LIST)
        {
            auto* valuePtr = (EDoubleList*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
        else if(m_type == EVariant::EVariantType::STRING_LIST)
        {
            auto* valuePtr = (EStringList*)value_ptr;
            delete valuePtr;
            value_ptr = NULL;
        }
    }
    else
    {
        value_ptr = NULL;
    }
    
    m_type = EVariantType::EMPTY;

}

EVariant& EVariant::operator=(int value)
{
    this->clear();

    m_type = EVariant::EVariantType::INT;
    value_int = value;
    return *this;
}

EVariant& EVariant::operator=(long value)
{
    this->clear();

    m_type = EVariant::EVariantType::LONG;
    value_long = value;
    return *this;
}

EVariant& EVariant::operator=(double value)
{
    this->clear();

    m_type = EVariant::EVariantType::DOUBLE;
    value_double = value;
    return *this;
}

EVariant& EVariant::operator=(bool value)
{
    this->clear();

    m_type = EVariant::EVariantType::BOOL;
    value_bool = value;
    return *this;
}

EVariant& EVariant::operator=(const EVec2& value)
{
    if(m_type != EVariant::EVariantType::VEC2)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::VEC2;
    EVec2* vec2Ptr = (EVec2*)value_ptr;

    if(vec2Ptr == NULL)
    {
        vec2Ptr = new EVec2();
    }

    *vec2Ptr = value;

    value_ptr = (intptr_t)vec2Ptr;

    return *this;
}

EVariant& EVariant::operator=(const EVec3& value)
{
    if(m_type != EVariant::EVariantType::VEC3)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::VEC3;
    EVec3* vec3Ptr = (EVec3*)value_ptr;

    if(vec3Ptr == NULL)
    {
        vec3Ptr = new EVec3();
    }

    *vec3Ptr = value;

    value_ptr = (intptr_t)vec3Ptr;

    return *this;
}

EVariant& EVariant::operator=(const EVec4& value)
{
    if(m_type != EVariant::EVariantType::VEC4)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::VEC4;
    EVec4* vec4Ptr = (EVec4*)value_ptr;

    if(vec4Ptr == nullptr)
    {
        vec4Ptr = new EVec4();
    }
    *vec4Ptr = value;

    value_ptr = (intptr_t)vec4Ptr;

    return *this;
}

EVariant& EVariant::operator=(const EColor& value)
{
    if(m_type != EVariant::EVariantType::COLOR)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::COLOR;
    EColor* colorPtr = (EColor*)value_ptr;

    if(colorPtr == nullptr)
    {
        colorPtr = new EColor();
    }
    *colorPtr = value;

    value_ptr = (intptr_t)colorPtr;

    return *this;
}

EVariant& EVariant::operator=(const EMatrix4x4& value)
{
    if( m_type != EVariant::EVariantType::MATRIX_4X4)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::MATRIX_4X4;

    EMatrix4x4* matPtr = (EMatrix4x4*)value_ptr;

    if (matPtr == nullptr)
    {
        matPtr = new EMatrix4x4();
    }
    *matPtr = value;

    value_ptr = (intptr_t)matPtr;

    return *this;
}

EVariant& EVariant::operator=(const char* value)
{
    if(m_type != EVariant::EVariantType::STRING)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::STRING;
    EString* strPtr = (EString*)value_ptr;

    if (strPtr == nullptr)
    {
        strPtr = new EString();
    }
    *strPtr = value;

    value_ptr = (intptr_t)strPtr;

    return *this;
}

EVariant& EVariant::operator=(const EString& value)
{
    if(m_type != EVariant::EVariantType::STRING)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::STRING;
    EString* strPtr = (EString*)value_ptr;

    if (strPtr == nullptr)
    {
        strPtr = new EString();
    }
    *strPtr = value;

    value_ptr = (intptr_t)strPtr;

    return *this;
}

EVariant& EVariant::operator=(const EIntList& value)
{
    if (m_type != EVariant::EVariantType::INT_LIST)
        this->clear();

    m_type = EVariant::EVariantType::INT_LIST;
    auto* vecPtr = (EIntList*)value_ptr;
    if (vecPtr == nullptr)
        vecPtr = new EIntList();

    *vecPtr = value;
    value_ptr = (intptr_t)vecPtr;
    return *this;
}

EVariant& EVariant::operator=(const EDoubleList& value)
{
    if (m_type != EVariant::EVariantType::DOUBLE_LIST)
        this->clear();

    m_type = EVariant::EVariantType::DOUBLE_LIST;
    auto* vecPtr = (EDoubleList*)value_ptr;
    if (vecPtr == nullptr)
        vecPtr = new EDoubleList();

    *vecPtr = value;
    value_ptr = (intptr_t)vecPtr;
    return *this;
}

EVariant& EVariant::operator=(const EStringList& value)
{
    if (m_type != EVariant::EVariantType::STRING_LIST)
        this->clear();

    m_type = EVariant::EVariantType::STRING_LIST;
    auto* vecPtr = (EStringList*)value_ptr;
    if (vecPtr == nullptr)
        vecPtr = new EStringList();

    *vecPtr = value;
    value_ptr = (intptr_t)vecPtr;
    return *this;
}

#ifdef _WIN32
EVariant& EVariant::operator=(intptr_t value)
{
    if(m_type != EVariant::EVariantType::INTPTR_T)
    {
        this->clear();
    }

    m_type = EVariant::EVariantType::INTPTR_T;

    value_ptr = value;

    return *this;
}
#endif

EVariant& EVariant::operator=(const EVariant& value)
{
    if (this == &value) return *this;
    this->clear();
    m_type = value.m_type;
    if(m_type == EVariantType::BOOL)
    {
        value_bool = value.value_bool;
    }
    else if(m_type == EVariantType::DOUBLE)
    {
        value_double = value.value_double;
    }
    else if(m_type == EVariantType::INT)
    {
        value_int = value.value_int;
    }
    else if(m_type == EVariantType::LONG)
    {
        value_long = value.value_long;
    }
    else if(m_type == EVariantType::MATRIX_4X4)
    {
        EMatrix4x4* tempPtr = (EMatrix4x4*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::VEC2)
    {
        EVec2* tempPtr = (EVec2*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::VEC3)
    {
        EVec3* tempPtr = (EVec3*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::VEC4)
    {
        EVec4* tempPtr = (EVec4*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::COLOR)
    {
        EColor* tempPtr = (EColor*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::STRING)
    {
        EString* tempPtr = (EString*)value.value_ptr;
        *this = *tempPtr;
    }
    else if (m_type == EVariantType::INTPTR_T)
    {
        this->value_ptr = value.value_ptr;
    }
    else if(m_type == EVariantType::VARIANT)
    {
        // ⚠️ 不能写 `*this = EVariant(*tempPtr, AsVariant);` —— 那会回到**本**重载(此时 m_type 已被置为
        //    VARIANT ⇒ 又进本分支) ⇒ 无限递归, 实测栈溢出 0xC00000FD(ExeCommon 的 "VARIANT copy" 用例)。
        //    深拷贝内层即可: 内层的 m_type 是被包裹的真实类型(INT/STRING/…), 不会再进本分支。
        //    (其它分支写 `*this = *tempPtr;` 没事, 因为解析到的是**按具体类型**的 operator= 重载。)
        EVariant* tempPtr = (EVariant*)value.value_ptr;
        value_ptr = tempPtr ? (intptr_t)(new EVariant(*tempPtr)) : (intptr_t)0;
    }
    else if(m_type == EVariantType::INT_LIST)
    {
        auto* tempPtr = (EIntList*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::DOUBLE_LIST)
    {
        auto* tempPtr = (EDoubleList*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::STRING_LIST)
    {
        auto* tempPtr = (EStringList*)value.value_ptr;
        *this = *tempPtr;
    }
    else if(m_type == EVariantType::EMPTY)
    {
        this->clear();
    }

    return *this;
}

const int& EVariant::to_int() const
{
    return value_int;
}

int& EVariant::to_int()
{
    return value_int;
}

const long& EVariant::to_long() const
{
    return value_long;
}

long& EVariant::to_long()
{
    return value_long;
}

const double& EVariant::to_double() const
{
    return value_double;
}

double& EVariant::to_double()
{
    return value_double;
}

const bool& EVariant::to_bool() const
{
    return value_bool;
}

bool& EVariant::to_bool()
{
    return value_bool;
}

const EVec2& EVariant::to_EVec2() const
{
    EVec2* tempPtr = (EVec2*)value_ptr;
    return *tempPtr;
}

EVec2& EVariant::to_EVec2()
{
    EVec2* tempPtr = (EVec2*)value_ptr;
    return *tempPtr;
}

const EVec3& EVariant::to_EVec3() const
{
    EVec3* tempPtr = (EVec3*)value_ptr;
    return *tempPtr;
}

EVec3& EVariant::to_EVec3()
{
    EVec3* tempPtr = (EVec3*)value_ptr;
    return *tempPtr;
}

const EVec4& EVariant::to_EVec4() const
{
    EVec4* tempPtr = (EVec4*)value_ptr;
    return *tempPtr;
}

EVec4& EVariant::to_EVec4()
{
    EVec4* tempPtr = (EVec4*)value_ptr;
    return *tempPtr;
}

const EColor& EVariant::to_EColor() const
{
    EColor* tempPtr = (EColor*)value_ptr;
    return *tempPtr;
}

EColor& EVariant::to_EColor()
{
    EColor* tempPtr = (EColor*)value_ptr;
    return *tempPtr;
}

const EMatrix4x4& EVariant::to_EMatrix4x4() const
{
    EMatrix4x4* tempPtr = (EMatrix4x4*)value_ptr;
    return *tempPtr;
}

EMatrix4x4& EVariant::to_EMatrix4x4()
{
    EMatrix4x4* tempPtr = (EMatrix4x4*)value_ptr;
    return *tempPtr;
}

const EString& EVariant::to_EString() const
{
    EString* tempPtr = (EString*)value_ptr;
    return *tempPtr;
}

EString& EVariant::to_EString()
{
    EString* tempPtr = (EString*)value_ptr;
    return *tempPtr;
}

intptr_t EVariant::to_intptr_t() const
{
    return value_ptr;
}

const EVariant& EVariant::to_EVariant() const
{
    if (m_type == VARIANT) {
        EVariant* tempPtr = (EVariant*)value_ptr;
        return *tempPtr;
    }
    return *this;
}

EVariant& EVariant::to_EVariant()
{
    if (m_type == VARIANT) {
        EVariant* tempPtr = (EVariant*)value_ptr;
        return *tempPtr;
    }
    return *this;
}

const EIntList& EVariant::to_EIntList() const
{
    auto* tempPtr = (EIntList*)value_ptr;
    return *tempPtr;
}

EIntList& EVariant::to_EIntList()
{
    auto* tempPtr = (EIntList*)value_ptr;
    return *tempPtr;
}

const EDoubleList& EVariant::to_EDoubleList() const
{
    auto* tempPtr = (EDoubleList*)value_ptr;
    return *tempPtr;
}

EDoubleList& EVariant::to_EDoubleList()
{
    auto* tempPtr = (EDoubleList*)value_ptr;
    return *tempPtr;
}

const EStringList& EVariant::to_EStringList() const
{
    auto* tempPtr = (EStringList*)value_ptr;
    return *tempPtr;
}

EStringList& EVariant::to_EStringList()
{
    auto* tempPtr = (EStringList*)value_ptr;
    return *tempPtr;
}

EString EVariant::getStrForDebug()
{
    EString str = "EVariant, Type=";
    if(m_type == EVariantType::BOOL)
    {
        str += "Bool";
        str += " value=";
        if(value_bool == true)
        {
            str += "true";
        }
        else if(value_bool == false)
        {
            str += "false";
        }
    }
    else if(m_type == EVariantType::DOUBLE)
    {
        str += "Double";
        str += EString(" value=") + std::to_string(value_double);
    }
    else if(m_type == EVariantType::INT)
    {
        str += "Int";
        str += EString(" value=") + std::to_string(value_int);
    }
    else if(m_type == EVariantType::MATRIX_4X4)
    {
        EMatrix4x4* tempPtr = (EMatrix4x4*)value_ptr;
        str += "Matrix_4X4";
        // 参考系(不影响 16 个数, 但看日志时是关键信息)
        str += (tempPtr->frame == EMatrix4x4::WORLD) ? " frame=World" : " frame=Local";
        str += EString(" value=\n");
        for(int j = 0; j < 4;j++)
        {
            str += std::to_string(tempPtr->data[0][j])
            + ","
            + std::to_string(tempPtr->data[1][j])
            + ","
            + std::to_string(tempPtr->data[2][j])
            + ","
            + std::to_string(tempPtr->data[3][j])
            +"\n";
        }
    }
    else if(m_type == EVariantType::VEC2)
    {
        EVec2* tempPtr = (EVec2*)value_ptr;
        str += "Vec2";
        str += EString(" value=") 
            + std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y);
    }
    else if(m_type == EVariantType::VEC3)
    {
        EVec3* tempPtr = (EVec3*)value_ptr;
        str += "Vec3";
        str += EString(" value=") 
            + std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z);
    }
    else if(m_type == EVariantType::VEC4)
    {
        EVec4* tempPtr = (EVec4*)value_ptr;
        str += "Vec4";
        str += EString(" value=") 
            + std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z)
            + ","
            + std::to_string(tempPtr->w);
    }
    else if(m_type == EVariantType::COLOR)
    {
        EColor* tempPtr = (EColor*)value_ptr;
        str += "Color";
        str += EString(" value=") 
            + std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z)
            + ","
            + std::to_string(tempPtr->w);
    }
    else if(m_type == EVariantType::STRING)
    {
        EString* tempPtr = (EString*)value_ptr;
        str += "String";
        str += EString(" value=\"") + *tempPtr + EString("\"");
    }
    else if(m_type == EVariantType::VARIANT)
    {
        EVariant* tempPtr = (EVariant*)value_ptr;
        str += "Variant";
        str += EString(" inner=(") + tempPtr->getStrForDebug() + EString(")");
    }
    else if(m_type == EVariantType::INT_LIST)
    {
        auto* vecPtr = (EIntList*)value_ptr;
        str += "IntList";
        str += " value=[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += std::to_string((*vecPtr)[i]);
        }
        str += "]";
    }
    else if(m_type == EVariantType::DOUBLE_LIST)
    {
        auto* vecPtr = (EDoubleList*)value_ptr;
        str += "DoubleList";
        str += " value=[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += std::to_string((*vecPtr)[i]);
        }
        str += "]";
    }
    else if(m_type == EVariantType::STRING_LIST)
    {
        auto* vecPtr = (EStringList*)value_ptr;
        str += "StringList";
        str += " value=[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += "\"" + (*vecPtr)[i] + "\"";
        }
        str += "]";
    }
    else if(m_type == EVariantType::EMPTY)
    {
        str += "Empty";
    }

    return str;

}

EString EVariant::getStrForDisplay()
{
    EString str;
    if(m_type == EVariantType::BOOL)
    {
        if(value_bool == true)
        {
            str = "true";
        }
        else if(value_bool == false)
        {
            str = "false";
        }
    }
    else if(m_type == EVariantType::DOUBLE)
    {
        str = std::to_string(value_double);
    }
    else if(m_type == EVariantType::INT)
    {
        str = std::to_string(value_int);
    }
    else if(m_type == EVariantType::MATRIX_4X4)
    {
        EMatrix4x4* tempPtr = (EMatrix4x4*)value_ptr;
        str += "Matrix_4X4";
        // 参考系(不影响 16 个数, 但看日志时是关键信息)
        str += (tempPtr->frame == EMatrix4x4::WORLD) ? " frame=World" : " frame=Local";
        str += EString(" value=\n");
        for(int j = 0; j < 4;j++)
        {
            str += std::to_string(tempPtr->data[0][j])
            + ","
            + std::to_string(tempPtr->data[1][j])
            + ","
            + std::to_string(tempPtr->data[2][j])
            + ","
            + std::to_string(tempPtr->data[3][j])
            +"\n";
        }
    }
    else if(m_type == EVariantType::VEC2)
    {
        EVec2* tempPtr = (EVec2*)value_ptr;
        str = std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y);
    }
    else if(m_type == EVariantType::VEC3)
    {
        EVec3* tempPtr = (EVec3*)value_ptr;
        str = std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z);
    }
    else if(m_type == EVariantType::VEC4)
    {
        EVec4* tempPtr = (EVec4*)value_ptr;
        str = std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z)
            + ","
            + std::to_string(tempPtr->w);
    }
    else if(m_type == EVariantType::COLOR)
    {
        EColor* tempPtr = (EColor*)value_ptr;
        str +=  std::to_string(tempPtr->x)
            + ","
            + std::to_string(tempPtr->y)
            + ","
            + std::to_string(tempPtr->z)
            + ","
            + std::to_string(tempPtr->w);
    }
    else if(m_type == EVariantType::STRING)
    {
        EString* tempPtr = (EString*)value_ptr;
        str = *tempPtr;
    }
    else if(m_type == EVariantType::VARIANT)
    {
        EVariant* tempPtr = (EVariant*)value_ptr;
        str += "Variant";
        str += EString(" inner=(") + tempPtr->getStrForDebug() + EString(")");
    }
    else if(m_type == EVariantType::INT_LIST)
    {
        auto* vecPtr = (EIntList*)value_ptr;
        str = "[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += std::to_string((*vecPtr)[i]);
        }
        str += "]";
    }
    else if(m_type == EVariantType::DOUBLE_LIST)
    {
        auto* vecPtr = (EDoubleList*)value_ptr;
        str = "[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += std::to_string((*vecPtr)[i]);
        }
        str += "]";
    }
    else if(m_type == EVariantType::STRING_LIST)
    {
        auto* vecPtr = (EStringList*)value_ptr;
        str = "[";
        for (size_t i = 0; i < vecPtr->size(); ++i)
        {
            if (i > 0) str += ", ";
            str += "\"" + (*vecPtr)[i] + "\"";
        }
        str += "]";
    }
    else if(m_type == EVariantType::EMPTY)
    {
        str = "Empty";
    }

    return str;

}
