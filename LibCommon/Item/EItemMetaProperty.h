// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef EITEM_META_PROPERTY_H
#define EITEM_META_PROPERTY_H

#include <DefineLibCommon.h>
#include <ECommonDefines.h>
#include <cstdint>
#include <map>

/**EItem 属性元信息: 名称/类型/当前值/默认值/范围等;
 * 由 EItemMeta 统一持有, 属性面板/序列化/撤销等共用这份元信息。*/
struct EItemMetaProperty;
typedef ESmartPtr<EItemMetaProperty> EItemMetaPropertyPtr;
struct COMMON_DLL EItemMetaProperty : public Referenced
{
    EString name;
    EString nameCN;
    EString typeName = "empty";
    EVariant value;
    bool ifReadOnly = false;
    bool ifShowUI = true;
    /**UI 是否每帧读取该属性以同步显示值。*/
    bool ifSyncUIEveryFrame = false;
    /**为 true 时使用 enumNames 作为枚举候选。*/
    bool ifEnum = false;
    /**E...Ptr 成员标记(反射生成器写入): 值为指向另一 EItem 的引用,
     * 属性面板提供 Item 引用选择器;普通 intptr_t(如 m_attached 指向 UI 树节点)
     * 保持 false, 面板按内嵌控件显示——面板不对 intptr_t 值做类型推断。*/
    bool ifItemRef = false;
    /**ifItemRef 时的可引用类型名(声明类型去掉 "Ptr", 如 EMyItemPtr→"EMyItem"):
     * 引用选择器按 is-a 过滤候选;非引用属性为空串。*/
    EString refClassName;
    EString groupName;
    EVariant defaultValue;
    EVariant minValue;
    EVariant maxValue;
    EString unitName;
    std::map<int, EString> enumNames;
    EString description;///< 中文说明(头文件 \brief 段)
    EString descriptionEN;///< 英文说明(头文件 \~english 段; 旧注释为空 ⇒ 显示回退中文)
    ///< 头文件声明顺序(反射生成时写入); 0 表示未赋值(手动注册)
    int index = 0;
    ///< meta 内注册顺序,EItemMeta::addProperty 时全局递增;用于组间排序(父类成员先注册)
    int regOrder = 0;


    typedef void (*Fun_setProperty)(intptr_t m_owner, const EVariant&);
    typedef void (*Fun_getProperty)(intptr_t m_owner, EVariant&);

    void setPropertyValue(const EVariant& value);           // 仅设置属性值(写缓存并调用 setter, 不触发通知)
    void setPropertyValueAndNotify(const EVariant& value);  // 设置属性值并更新矩阵变更序号(通知由上层负责)
    void getPropertyValue(EVariant& value);

    /**ui_matrix 变更序号(全局单调递增): 每次矩阵属性经 setPropertyValueAndNotify 写值就 +1。
     * 供需要判断"父链变换可能已变"的读取方免去每帧重算。*/
    static uint64_t getMatrixChangeSeq();

    Fun_setProperty m_funSetProperty = nullptr;
    Fun_getProperty m_funGetProperty = nullptr;

    intptr_t m_owner = 0;

    long getTag();
};


#endif // EITEM_META_PROPERTY_H 
