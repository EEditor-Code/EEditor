// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_ITEM_META_H
#define E_ITEM_META_H

#include <Variant/EString.h>
#include "SmartPtr/ESmartPtr.h"
#include <DefineLibCommon.h>
#include <ECommonDefines.h>

#include <Item/EItemMetaProperty.h>
#include <Item/EItemMetaFunction.h>

struct EItemMetaData;
class EItemMeta;
typedef ESmartPtr<EItemMeta> EItemMetaPtr;
/**This class is used to represent a class's meta information,
 * which is same with QMetaObject. This can:
 * 1.creat a new class's object
 * 2.
 * */
class COMMON_DLL EItemMeta : public Referenced
{
public:
    EItemMeta();
    virtual ~EItemMeta();

    EItemMeta(const EItemMeta&) = delete;
    EItemMeta& operator=(const EItemMeta&) = delete;

    EString getClassName();
    void setClassName(const EString& className);

    EString getClassDescription();
    void setClassDescription(const EString& className);

    void addProperty(const EItemMetaPropertyPtr& p);
    void addProperty(const EString& name, const EVariant& value);

    EItemMetaPropertyPtr getProperty(int index);
    EItemMetaPropertyPtr getProperty(const EString& name);

    size_t getPropertyCount();
    
    void addFunction(const EItemMetaFunctionPtr& fun);
    EItemMetaFunctionPtr getFunction(int index);
    EItemMetaFunctionPtr getFunction(const EString& name);
    size_t getFunctionCount();

    EString getStrForDebug();

    // === Allowed Child Types(子项白名单; **三态**) ===
    //
    //   从未调用过该 setter  ⇒ 不限制(默认, 向后兼容)
    //   传**非空**列表       ⇒ 只允许这些类型(UI 的 Add 类型列表按它过滤)
    //   传**空**列表         ⇒ **明确禁止挂任何子项**
    //
    //   "从未设置"与"设为空"靠一个标志区分, 该标志放在 pimpl(EItemMetaData)里
    //   ⇒ 不改 EItemMeta 的 sizeof/vtable, 不破坏 ABI(每个插件的 autogen 都会 new EItemMeta)。
    /**设置允许的子类型白名单。传空列表 = 明确禁止挂任何子项(见上面的三态说明)。*/
    void setAllowedChildTypes(const std::vector<std::string>& types);
    std::vector<std::string> getAllowedChildTypes() const;
    /**是否**设置过**白名单。注意: 设置过也可能是空列表(= 禁止挂子项, 见 isNoChildAllowed)。
     *  从未设置返回 false。*/
    bool hasAllowedChildTypes() const;
    /**是否明确禁止挂任何子项(设置过白名单且列表为空)*/
    bool isNoChildAllowed() const;

    /**"禁止挂子项"专用的过滤串。
     *  UI 的类型过滤是 `类型路径.contains(tag)` **子串匹配**, 且空组会被 DialogTypeList 当
     *  "无约束"跳过 ⇒ 想表达"什么都不允许"只能拿一个**永不出现**的串去过滤。
     *  集中放在这里, 免得各调用点自己编串。*/
    static const char* const kNoChildFilterTag;

    // === Parent Class Names(父类链; 反射生成器在类型节点上写入完整祖先名列表) ===
    void setParentClassNames(const std::vector<std::string>& names);
    std::vector<std::string> getParentClassNames() const;

    // === 属性组可见性(属性面板按"声明类"整组隐藏) ===
    //
    // 组名 = 反射生成时写入的 prop->groupName / fun->getGroupName(), 对 autogen 成员
    // 即为**声明该成员的类名**(EItem / EUIAppSetting / ...)。
    // 本接口只作用于当前 meta 实例; EUIProperty 会沿当前项的父链查询 ⇒ 在祖先项上
    // 设置后对整棵子树生效(如 Setting Root 隐藏基类 EItem 组)。
    // 仅影响属性面板显示, 不改 meta 数据/RPC/序列化。
    void setPropertyGroupVisible(const EString& groupName, bool visible);
    bool isPropertyGroupVisible(const EString& groupName) const;

protected:
    EString m_className;
    EString m_classDescription;
    EItemMetaData* m_data;
    std::vector<std::string> m_allowedChildTypes;
    std::vector<std::string> m_parentClassNames;
};

#endif // E_ITEM_META_H
