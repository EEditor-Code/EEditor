// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_ITEM_META_FUNCTION_H
#define E_ITEM_META_FUNCTION_H

#include <cstdint>
#include <stdarg.h>
#include "EString.h"
#include "SmartPtr/ESmartPtr.h"
// #include <functional>
#include <DefineLibCommon.h>
#include <DefineCommon.h>
#include <EItemMetaProperty.h>
#include <EVariant.h>
/*This delegate code is not used in final version, but the delegate.h file is still in this project*/
// #include <delegate/delegate.h>

/**This class is used to represent a class's functions,
 * which is same with QMetaMethod. This can:
 * 1.get the parameter list.
 * 2.get return type 
 * 3.called by outside
 * */
struct EItemMetaFunctionData;
class EItemMetaFunction;
typedef ESmartPtr<EItemMetaFunction> EItemMetaFunctionPtr;
class COMMON_DLL EItemMetaFunction : public Referenced
{
public:
    enum FunType
    {
        FREE,
        CONSTRUCTOR,
        CLASS_MEMBER
    };

    /**This class is used to represent a class's functions,*/
    EItemMetaFunction();
    virtual ~EItemMetaFunction();

    void addProperty(const EItemMetaPropertyPtr& p);
    EItemMetaPropertyPtr getProperty(int index);
    size_t getPropertyCount();

    void setOwner(intptr_t owner);
    intptr_t getOwner();

    void setFunType(FunType type);
    FunType getFunType();

    void setName(const EString& name);
    EString getName();

    void setGroupName(const EString& name);
    EString getGroupName();

    void setIndex(int index);
    int getIndex();

    void setRegOrder(int order);
    int getRegOrder();

    void setReturnProperty(const EItemMetaPropertyPtr& p);
    EItemMetaPropertyPtr getReturnProperty();

    void call( EVariant* result, EList<EVariant*> vList);
    void call( EVariant* result);
    void call( EVariant* result, EVariant* v1);
    void call( EVariant* result, EVariant* v1, EVariant* v2);
    void call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3);
    void call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3, EVariant* v4);
    void call( EVariant* result, EVariant* v1, EVariant* v2, EVariant* v3, EVariant* v4, EVariant* v5);

    // std::function<EVariant()> m_fun_0;
    // std::function<EVariant(EVariant)> m_fun_1;
    // std::function<EVariant(EVariant,EVariant)> m_fun_2;
    // std::function<EVariant(EVariant,EVariant,EVariant)> m_fun_3;
    // std::function<EVariant(EVariant,EVariant,EVariant)> m_fun_4;
    // std::function<EVariant(EVariant,EVariant,EVariant,EVariant,EVariant)> m_fun_5;


    /**if FunType==CLASS_MEMBER, this m_owner is the object pointer*/
    FunType m_funType;
    EString m_name;
    EString nameCN;///< 中文显示名(头文件注释 \nameCN; 空 = 显示回退 i18n 表/代码名)
    EString m_groupName;
    /**头文件文档注释(autogen 生成时写入; 空串 = 无注释 ⇒ UI 不显示说明入口)。
     * 与 EItemMetaProperty::description 同名同义 —— 生成代码保持 "->description =" 一致形式*/
    EString description;
    EString descriptionEN;///< 英文说明(\~english 段; 空 ⇒ 显示回退中文)
    int m_index = 0;///< 头文件声明顺序,autogen 生成时写入;0 表示未赋值(手动注册)
    int m_regOrder = 0;///< meta 内注册顺序,EItemMeta::addFunction 时全局递增;用于组间排序(父类成员先注册)

    long getTag();
protected:
    /**Subclass must rewrite this function
     * owner usually be nullptr even for class member function*/
    virtual void callInternal( intptr_t owner, ... );

    EItemMetaFunctionData* m_data;

};

#endif // E_ITEM_META_FUNCTION_H
