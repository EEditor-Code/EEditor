// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EItemMeta.h"
#include "EItemMetaFunction.h"
#include <set>

/** EVariant::EVariantType → 类型名(下标必须与 EVariant.h 的枚举顺序**逐一对应**)。
 *  ⚠️ 此表曾停留在旧枚举(缺 VEC2/VARIANT/列表/FILE_INFO), 从 VEC2 起全部错位且越界;
 *  新 JSON 序列化按 typeName 回退注册未知属性, 故在此校正并加静态断言锁死长度。*/
static const char* EItemMeta_typeStrArray[] = {
    "int",          // INT
    "double",       // DOUBLE
    "bool",         // BOOL
    "long",         // LONG
    "EVec2",        // VEC2
    "EVec3",        // VEC3
    "EVec4",        // VEC4
    "EColor",       // COLOR
    "EMatrix4x4",   // MATRIX_4X4
    "EString",      // STRING
    "intptr_t",     // INTPTR_T
    "empty",        // EMPTY
    "EVariant",     // VARIANT
    "EIntList",     // INT_LIST
    "EDoubleList",  // DOUBLE_LIST
    "EStringList"   // STRING_LIST
};
static_assert(sizeof(EItemMeta_typeStrArray) / sizeof(EItemMeta_typeStrArray[0])
                  == EVariant::STRING_LIST + 1,
              "EItemMeta_typeStrArray must match EVariant::EVariantType");

struct EItemMetaData
{
    EList<EItemMetaPropertyPtr> m_propertys;
    EMap<EString, size_t> m_mapPropFastIndex;
    EList<EItemMetaFunctionPtr> m_functions;
    EMap<EString, size_t> m_mapFunFastIndex;
    ///< 成员注册序号计数器:addFunction/addProperty 时全局递增,
    ///< 父类成员(基类构造时)先注册,故注册序天然反映"父类在前"
    int m_regOrderCounter = 0;

    /**是否**调用过** setAllowedChildTypes。
     *  放在 pimpl 里(而不是 EItemMeta 的直接成员)是**故意的**: 每个插件的 autogen 都会
     *  `new EItemMeta()`, 一旦 sizeof(EItemMeta) 变了, 全部插件都得重编。
     *  这里加成员只改这个 .cpp 私有的结构体 ⇒ ABI 不变。*/
    bool allowedChildTypesSet = false;

    /**属性面板中整组隐藏的组名(= 声明类名); 同样放 pimpl 保 ABI。*/
    std::set<EString> hiddenPropertyGroups;
};

EItemMeta::EItemMeta()
{
    m_data = new EItemMetaData;
}

EItemMeta::~EItemMeta()
{
    delete m_data;
}

EString EItemMeta::getClassName()
{
    return m_className;
}

void EItemMeta::setClassName(const EString& className)
{
    m_className = className;
}


EString EItemMeta::getClassDescription()
{
    return m_classDescription;
}

void EItemMeta::setClassDescription(const EString& classDescription)
{
    m_classDescription = classDescription;
}

void EItemMeta::addProperty(const EItemMetaPropertyPtr& p)
{
    p->regOrder = ++m_data->m_regOrderCounter;
    m_data->m_propertys.push_back(p);
    m_data->m_mapPropFastIndex[p->name] = m_data->m_propertys.size()-1;
}


void EItemMeta::addProperty(const EString& name, const EVariant& value)
{
    // EItemMetaPropertyPtr newItem = NewSmartPtr(EItemMetaProperty)();
    EItemMetaPropertyPtr newItem = new EItemMetaProperty();
    newItem->typeName = EItemMeta_typeStrArray[value.m_type];
    newItem->name = name;
    newItem->value = value;
    
    this->addProperty(newItem);
}

EItemMetaPropertyPtr EItemMeta::getProperty(int index)
{
    EItemMetaPropertyPtr result = nullptr;
    if(index >=0 && index <m_data->m_propertys.size())
    {
        result = m_data->m_propertys[index];
    }
    return result;
}

EItemMetaPropertyPtr EItemMeta::getProperty(const EString& name)
{
    EItemMetaPropertyPtr result = nullptr;
    if(m_data->m_mapPropFastIndex.find(name) != m_data->m_mapPropFastIndex.end())
    {
        size_t index = m_data->m_mapPropFastIndex[name];
        if(index >=0 && index < m_data->m_propertys.size())
        {
            result = m_data->m_propertys[index];
        }
    }
    return result;
}

size_t EItemMeta::getPropertyCount()
{
    return m_data->m_propertys.size();
}
    
void EItemMeta::addFunction(const EItemMetaFunctionPtr& fun)
{
    fun->setRegOrder(++m_data->m_regOrderCounter);
    m_data->m_functions.push_back(fun);
    m_data->m_mapFunFastIndex[fun->m_name] = m_data->m_functions.size()-1;
}

EItemMetaFunctionPtr EItemMeta::getFunction(int index)
{
    EItemMetaFunctionPtr result = nullptr;
    if(index >=0 && index <m_data->m_functions.size())
    {
        result = m_data->m_functions[index];
    }
    return result;
}

EItemMetaFunctionPtr EItemMeta::getFunction(const EString& name)
{
    EItemMetaFunctionPtr result = nullptr;
    if(m_data->m_mapFunFastIndex.find(name) != m_data->m_mapFunFastIndex.end())
    {
        size_t index = m_data->m_mapFunFastIndex[name];
        if(index >=0 && index < m_data->m_functions.size())
        {
            result = m_data->m_functions[index];
        }
    }
    return result;
}

size_t EItemMeta::getFunctionCount()
{
    return m_data->m_functions.size();
}

EString EItemMeta::getStrForDebug()
{
    //TODO:the return value str is may be released for example in ELOG_DEBUG function:
    //ELOG_DEBUG("str=%s", itemMeta->getStrForDebug().data())
    //In windows, code above will output empty content, because the returned EString is a function local value and its data is released.
    //In Linux, this code is right;
    EString str = "EItemMeta information:\n";

    str += EString("className=")+this->m_className+"\n";
    str += EString("property count=")+std::to_string(this->m_data->m_propertys.size())+"\n";

    for(size_t i =0 ; i < this->m_data->m_propertys.size(); i++ )
    {
        str += std::to_string(i) + " ";
        str += this->m_data->m_propertys[i]->name + " ";
        str += this->m_data->m_propertys[i]->value.getStrForDebug()+" ";
        str += this->m_data->m_propertys[i]->groupName;
        str += "\n";
    }

    str += EString("function count=")+std::to_string(this->m_data->m_functions.size())+"\n";

    for(size_t i =0 ; i < this->m_data->m_functions.size(); i++ )
    {
        str += std::to_string(i) + " ";
        str += this->m_data->m_functions[i]->m_name + " ";
        str += this->m_data->m_functions[i]->getReturnProperty()->typeName + " ";
        str += "(";
        for(size_t j = 0 ; j < this->m_data->m_functions[i]->getPropertyCount(); j++)
        {
            str += this->m_data->m_functions[i]->getProperty(j)->typeName + ",";
        }
        
        str += ")\n";
    }

    return str;
}

// === Allowed Child Types ===

const char* const EItemMeta::kNoChildFilterTag = "__no_child__";

void EItemMeta::setAllowedChildTypes(const std::vector<std::string>& types)
{
    m_allowedChildTypes = types;
    // ★ 空列表也算"设置过" —— 这正是"明确禁止挂子项"的表达方式(见头文件的三态说明)
    m_data->allowedChildTypesSet = true;
}

std::vector<std::string> EItemMeta::getAllowedChildTypes() const
{
    return m_allowedChildTypes;
}

bool EItemMeta::hasAllowedChildTypes() const
{
    // 语义: 由"列表非空"改为"**设置过**"(空列表 = 禁止, 也要让调用方知道)
    return m_data->allowedChildTypesSet;
}

bool EItemMeta::isNoChildAllowed() const
{
    return m_data->allowedChildTypesSet && m_allowedChildTypes.empty();
}

void EItemMeta::setParentClassNames(const std::vector<std::string>& names)
{
    m_parentClassNames = names;
}

std::vector<std::string> EItemMeta::getParentClassNames() const
{
    return m_parentClassNames;
}

void EItemMeta::setPropertyGroupVisible(const EString& groupName, bool visible)
{
    if (visible)
    {
        m_data->hiddenPropertyGroups.erase(groupName);
    }
    else
    {
        m_data->hiddenPropertyGroups.insert(groupName);
    }
}

bool EItemMeta::isPropertyGroupVisible(const EString& groupName) const
{
    return m_data->hiddenPropertyGroups.find(groupName) == m_data->hiddenPropertyGroups.end();
}
