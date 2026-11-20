// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef EITEM_META_PROPERTY_H
#define EITEM_META_PROPERTY_H

#include <DefineLibCommon.h>
#include <DefineCommon.h>
#include <cstdint>
// #include <functional>
// #include <delegate/delegate.h>

/**definations of item's property, the core is a Qt's QVariant object to wrap all c++ variable types.
 *class Item will referense this.
 * */
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
    // true, than this property is travered 
    // by ui's time enevt in every frame, 
    // to keep sync ui's value and  property's value
    bool ifSyncUIEveryFrame= false;
    //if this property is enum, than enumNames will be used
    bool ifEnum = false;
    /**E...Ptr 成员标记(反射生成器写入): 值为指向另一 EItem 的引用,
     * 属性面板提供 Item 引用选择器;普通 intptr_t(如 m_attached 指向 Qt 树节点)
     * 保持 false, 面板按内嵌控件显示——面板不对 intptr_t 值做类型推断。*/
    bool ifItemRef = false;
    /**ifItemRef 时的可引用类型名(声明类型去 "Ptr", 如 E3DPointCloudPtr→"E3DPointCloud"):
     * 引用选择器按 is-a 过滤候选(见 EItemManager::isTypeOf);非引用属性为空串。*/
    EString refClassName;
    EString groupName;
    EVariant defaultValue;
    EVariant minValue;
    EVariant maxValue;
    EString unitName;
    EMap<int, EString> enumNames;
    EString description;///< 中文说明(头文件 \brief 段)
    EString descriptionEN;///< 英文说明(头文件 \~english 段; 旧注释为空 ⇒ 显示回退中文)
    ///< 头文件声明顺序,autogen 生成时写入;0 表示未赋值(手动注册)
    int index = 0;
    ///< meta 内注册顺序,EItemMeta::addProperty 时全局递增;用于组间排序(父类成员先注册)
    int regOrder = 0;


    typedef void (*Fun_setProperty)(intptr_t m_owner, const EVariant&);
    typedef void (*Fun_getProperty)(intptr_t m_owner, EVariant&);

    void setPropertyValue(const EVariant& value);           // 仅设置属性值(不通知 model)
    void setPropertyValueAndNotify(const EVariant& value);  // 设置 + 通知 model 分发 ItemChanged 事件
    void getPropertyValue(EVariant& value);

    /**ui_matrix 变更序号(全局单调递增): 每次"矩阵属性"经 setPropertyValueAndNotify 写值就 +1。
     * 供需要"父链变换可能已变"判据的消费方免去每帧重算(如 E3DTransDrag 的世界矩阵缓存、
     * 3D 视图的父世界矩阵刷新)。注意: 只统计经属性通道的写入 —— 加载器直接改 vsg 节点不计,
     * 但换目标/装载后会重新取值, 不受影响。*/
    static uint64_t getMatrixChangeSeq();

    Fun_setProperty m_funSetProperty = NULL;
    Fun_getProperty m_funGetProperty = NULL;

    intptr_t m_owner;

    long getTag();

    // std::function<void(const EVariant& value)> funSetProperty;
    // std::function<void(EVariant& value)> funGetProperty;
    // void setOwner(intptr_t owner);
    // intptr_t getOwner();
};


#endif // EITEM_META_PROPERTY_H 
