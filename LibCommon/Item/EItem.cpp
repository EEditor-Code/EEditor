// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EItem.h"
#include "DefineCommon.h"
#include "EItemMeta.h"
#include "EItemMetaProperty.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <functional>
#include <unordered_set>
#include <vector>

namespace
{
std::atomic<int> g_nextItemId{1};
}

EItem::EItem()
{
    m_id = g_nextItemId.fetch_add(1);
    m_attached = NULL;
}

EItem::~EItem()
{
}

bool EItem::isSimulationParticipant() const
{
    return false;
}

void EItem::onSimulationStart()
{
}

void EItem::onSimulationPause()
{
}

void EItem::onSimulationResume()
{
}

void EItem::onSimulationStop()
{
}
    
void EItem::setName(const char* strName)
{
    m_name = strName;
}

void EItem::setName(const EString& strName)
{
    this->m_name = strName;
}

EString EItem::getName()
{
    return m_name;
}

EString EItem::getType()
{
    return m_type;
}

long EItem::getTag()
{
    return (long)this;
}

void EItem::setIfShownInUI(bool boolValue)
{
    this->m_ifShownInUI = boolValue;
}

bool EItem::getIfShownInUI()
{
    return this->m_ifShownInUI;
}

void EItem::setUIDeletable(bool boolValue)
{
    this->m_uiDeletable = boolValue;
}

bool EItem::isUIDeletable() const
{
    return this->m_uiDeletable;
}

void EItem::setDescription(const char* strName)
{
    this->m_description = strName;
}

EString EItem::getDescription()
{
    return this->m_description;
}

bool EItem::addChild(EItem* childItem)
{
    if (childItem == nullptr)
    {
        return false;
    }
    // 已挂 model: 走 model 通道(重复/循环校验 + begin/end 通知 + model 传播)
    return d_addChild(childItem);
}

bool EItem::isAncestorOf(const EItem* node) const
{
    if (node == nullptr)
    {
        return false;
    }
    std::vector<const EItem*> stack;
    std::unordered_set<const EItem*> visited;
    stack.push_back(node);
    while (!stack.empty())
    {
        const EItem* current = stack.back();
        stack.pop_back();
        if (!visited.insert(current).second)
        {
            continue;
        }
        for (EItem* parent : current->m_itemListParent)
        {
            if (parent == this)
            {
                return true;
            }
            if (parent != nullptr)
            {
                stack.push_back(parent);
            }
        }
    }
    return false;
}

bool EItem::d_addChild(EItem* child)
{
    if (child == nullptr)
    {
        return false;
    }
    if (child == this || child->isAncestorOf(this))
    {
        ELOG_ERROR("EItem: cannot insert an ancestor (or self) as a child");
        return false;
    }
    if (this->containsChild(child))
    {
        ELOG_ERROR("EItem: cannot add an ancestor (or self) as a child");
        return false;
    }
    if (this->containsChild(child))
    {
        ELOG_ERROR("Item \"%s\" is already a child of Item \"%s\",ignoring call",
                child->getName().data(), this->getName().data());
        return false;
    }
    m_itemListChild.push_back(child);
    child->m_itemListParent.push_back(this);
    return true;
}

bool EItem::insertChild(int index, EItem* child )
{
    if (child == nullptr)
        return false;
    return d_insertChild(index, child);
}

bool EItem::d_insertChild(int index, EItem* child)
{
    if (child == nullptr)
    {
        return false;
    }
    if (child == this || child->isAncestorOf(this))
    {
        ELOG_ERROR("EItem: cannot insert an ancestor (or self) as a child");
        return false;
    }
    if (this->containsChild(child))
    {
        ELOG_ERROR("Item \"%s\" is already a child of Item \"%s\",ignoring call",
                child->getName().data(), this->getName().data());
        return false;
    }

    EItemList& _children = m_itemListChild;

    if (index >= (int)_children.size())
    {
        index = (int)_children.size();      // set correct index value to be passed to the "childInserted" method
        _children.push_back(child);
    }
    else
    {
        if (index < 0)
        {
            index = 0;
        }
        _children.insert(_children.begin()+index, child);
    }

    // register as parent of child.
    // child->m_itemListParent.push_back(shared_from_this());
    child->m_itemListParent.push_back(this);
    return true;
}

EItem* EItem::getChild(int index)
{
    EItem* result = nullptr;
    if(index >=0 && index < m_itemListChild.size())
    {
        result = m_itemListChild[index].get();

    }
    return result;
}

EItem* EItem::getChildByTypeName(const EString& typeName)
{
    EItem* result = nullptr;
    for(int i = 0 ; i < this->m_itemListChild.size(); i++)
    {
        EString className = this->m_itemListChild[i]->getOrCreatMeta()->getClassName();
        if(typeName == className)
        {
            result = this->m_itemListChild[i].get();
            break;
        }
    }

    return result;
}

EItem* EItem::parent() const
{
    return m_itemListParent.empty() ? nullptr : m_itemListParent.front();
}

int EItem::row() const
{
    EItem* p = parent();
    return p ? p->getChildIndex(const_cast<EItem*>(this)) : -1;
}

EItem* EItem::getParent(int index)
{
    // EItemPtr result = nullptr;
    EItem* result = NULL;
    if(index >=0 && index < m_itemListParent.size())
    {
        result = m_itemListParent[index];
        // result = EItemPtr(m_itemListParent[index]);
    }
    return result;
}

int EItem::getChildIndex(EItem* childPtr)
{
    EItemList& _children = m_itemListChild;
    for (unsigned int childNum=0;childNum<_children.size();++childNum)
    {
        if (_children[childNum] == childPtr) 
        {
            return childNum;
        }
    }
    return -1; // node not found.
}

int EItem::getParentIndex(EItem* parentPtr)
{
    // std::vector<EItemPtr>& _parents = m_itemListParent;
    std::vector<EItem*>& _parents = m_itemListParent;
    for (unsigned int num=0; num<_parents.size();++num)
    {
        if (_parents[num] == parentPtr) 
        {
            return num;
        }
    }
    return -1; // node not found.
}

size_t EItem::getParentCount()
{
    return m_itemListParent.size();
}


size_t EItem::getChildCount()
{
    return m_itemListChild.size();
}

bool EItem::containsChild( const EItem* child )
{
    EItemList& _children = m_itemListChild;

    for (EItemList::const_iterator itr=_children.begin();
            itr!=_children.end();
            ++itr)
    {
        if (*itr == child) 
            return true;
    }
    return false;
}

bool EItem::removeChild(int index, int numChildrenToRemove)
{
    return d_removeChild(index, numChildrenToRemove);
}

bool EItem::attachChild(EItem* child)
{
    return d_attachChild(static_cast<int>(m_itemListChild.size()), child);
}

bool EItem::d_attachChild(int index, EItem* child)
{
    if (child == nullptr)
    {
        return false;
    }
    if (this->containsChild(child))
    {
        ELOG_ERROR("Item \"%s\" is already a child of Item \"%s\",ignoring call",
                child->getName().data(), this->getName().data());
        return false;
    }

    EItemList& _children = m_itemListChild;
    if (index >= (int)_children.size())
    {
        index = (int)_children.size();
        _children.push_back(child);
    }
    else
    {
        if (index < 0)
        {
            index = 0;
        }
        _children.insert(_children.begin() + index, child);
    }
    child->m_itemListParent.push_back(this);
    return true;
}

bool EItem::d_removeChild(int index, int numChildrenToRemove)
{
    if(index < 0 || index >= (int)m_itemListChild.size())
    {
        return false;
    }
    int endOfRemoveRange = index + numChildrenToRemove;
    if (endOfRemoveRange > (int)m_itemListChild.size())
    {
        endOfRemoveRange = (int)m_itemListChild.size();
    }

    //remove parent ptr in child item
    for(int i = index; i < endOfRemoveRange; i++)
    {
        EItemPtr child = m_itemListChild[i];
        //child->removeParent(shared_from_this());
        child->removeParent(this);
    }

    m_itemListChild.erase(m_itemListChild.begin()+index, m_itemListChild.begin()+endOfRemoveRange);
    return true;
}

bool EItem::removeParent(int index, int numParentToRemove)
{
    if (index < 0 || numParentToRemove <= 0 ||
        index >= static_cast<int>(m_itemListParent.size()))
    {
        return false;
    }
    int endOfRemoveRange = index + numParentToRemove;
    if (endOfRemoveRange > static_cast<int>(m_itemListParent.size()))
    {
        endOfRemoveRange = static_cast<int>(m_itemListParent.size());
    }
    m_itemListParent.erase(m_itemListParent.begin() + index,
                           m_itemListParent.begin() + endOfRemoveRange);
    return true;
}

bool EItem::removeChild(EItem* childPtr)
{
    EItemList& _children = m_itemListChild;
    int pos = getChildIndex(childPtr);
    if (pos >= 0 && pos<_children.size())
        return removeChild(pos,1);
    else 
        return false;
}

bool EItem::removeParent(EItem* parentPtr)
{
    // std::vector<EItemPtr>& _parents = m_itemListParent;
    std::vector<EItem*>& _parents = m_itemListParent;
    int pos = getParentIndex(parentPtr);
    if (pos >= 0 && pos<_parents.size())
        return removeParent(pos,1);
    else 
        return false;
}

bool EItem::removeChildrenAll()
{
    if (m_itemListChild.empty())
    {
        return false;
    }
    d_clearChildren();
    return true;
}

void EItem::d_clearChildren()
{
    if (m_itemListChild.empty())
    {
        return;
    }
    // 独立实现(不走 removeChild,避免重复事件): 仅移除父引用 + 清空
    for (size_t i = 0; i < m_itemListChild.size(); i++)
    {
        m_itemListChild[i]->removeParent(this);
    }
    m_itemListChild.clear();
}

void EItem::setAttached(intptr_t ptr)
{
    m_attached = ptr;
}

intptr_t EItem::getAttached()
{
    return m_attached;
}

void EItem::setIfHandledInTree(bool value)
{
    this->m_ifHandledInTree = value;
}

bool EItem::getIfHandledInTree()
{
    return this->m_ifHandledInTree;
}

EItemMeta* EItem::getOrCreatMeta()
{
    if(m_itemMeta == nullptr)
    {
        if(m_fun_createMeta != nullptr)
        {
            m_itemMeta = m_fun_createMeta(this);
        }
        else
        {
            m_itemMeta = new EItemMeta();
            m_itemMeta->setClassName(m_type);
        }
    }

    return m_itemMeta.get();
}

EItemMetaPtr EItem::getMeta() const
{
    return m_itemMeta;
}

void EItem::setMeta(EItemMetaPtr meta)
{
    m_itemMeta = meta;
}

void EItem::setFunCreateMeta(Fun_createMeta fun)
{
    m_fun_createMeta = fun;
}

EString EItem::getStrForDebug()
{
    EString str = "EItem information:\n";

    str += EString("name=")+this->m_name+"\n";
    str += EString("type=")+this->m_type+"\n";
    str += EString("child count=")+std::to_string(this->m_itemListChild.size())+"\n";
    str += EString("parent count=")+std::to_string(this->m_itemListParent.size())+"\n";

    return str;
}

void EItem::setPropertyIfShowUI(const EString& propName , bool value)
{
    EItemMetaPropertyPtr prop = this->getOrCreatMeta()->getProperty(propName);
    if(prop == nullptr){
        ELOG_INFO("prop is nullptr");
        return;
    }
    prop->ifShowUI = value;
}

void EItem::setPropertyIfSyncUIEveryFrame(const EString& propName, bool value)
{
    EItemMetaPropertyPtr prop = this->getOrCreatMeta()->getProperty(propName);
    if(prop == nullptr){
        return;
    }
    prop->ifSyncUIEveryFrame = value;
}

void EItem::setPropertyIconName(const EString& iconName)
{
    this->getOrCreatMeta()->addProperty("iconName", iconName);
}

EString EItem::getPropertyIconName()
{
    // 注:meta 在子类构造函数(autogen construct)中已创建,getOrCreatMeta 直接返回
    // 已有指针,无创建开销(实测与 getMeta 无差异;保持原实现)
    EItemMetaPropertyPtr prop = this->getOrCreatMeta()->getProperty("iconName");
    if(prop == nullptr)
    {
        return "";
    }
    return prop->value.to_EString();
}
// EString EItem::toTreeItemName(const EString& name)
// {
    // EString result;
    // if(name == "osg::Group")
    // {
        // result = "Group";
    // }
    // else if(name == "osg::Node")
    // {
        // result = "Node";
    // }
    // else if(name == "osg::MatrixTransform")
    // {
        // result = "MT";
    // }
    // else if(name == "osg::Geode")
    // {
        // result = "Geod";
    // }
    // else if(name == "osg::Geometry")
    // {
        // result = "Geom";
    // }
    // else if(name == "osgAnimation::Bone")
    // {
        // result = "Bone";
    // }
    // else
    // {
        // result = name;
    // }
    // return result;
// }


