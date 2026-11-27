// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#include "EItem.h"
#include "ECommonDefines.h"
#include "EItemMeta.h"
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>

namespace
{
std::atomic<int> g_nextItemId{1};
}

EItem::EItem()
{
    m_id = g_nextItemId.fetch_add(1);
}

EItem::~EItem()
{
    // 断开子项回指本项的裸指针，避免父项先析构后子项持有悬空父指针。
    // 注意：这里不能调 removeChild（会触发 unref 重入释放），
    // 只做列表清理，子项强引用由 m_itemListChild 析构时统一释放。
    for (const EItemPtr& child : m_itemListChild)
    {
        if (!child)
        {
            continue;
        }
        std::vector<EItem*>& parents = child->m_itemListParent;
        parents.erase(std::remove(parents.begin(), parents.end(), this), parents.end());
    }
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

void EItem::setDescription(const char* strName)
{
    this->m_description = strName;
}

EString EItem::getDescription()
{
    return this->m_description;
}

bool EItem::addChild(const EItemPtr& childItemRef)
{
    EItem* childItem = childItemRef.get();
    if (childItem == nullptr)
    {
        return false;
    }
    // 防环: 本项不能是 childItem 的后代(否则挂接后形成父子环)。
    if (childItem == this || isChildOf(childItemRef))
    {
        ELOG_ERROR("EItem: cannot add an ancestor (or self) as a child");
        return false;
    }

    // 重复挂载检查: 直接扫描子项列表, 不再走额外函数。
    for (const EItemPtr& child : m_itemListChild)
    {
        if (child.get() == childItem)
        {
            ELOG_ERROR("Item \"%s\" is already a child of Item \"%s\", ignoring call",
                    childItem->getName().data(), this->getName().data());
            return false;
        }
    }

    m_itemListChild.push_back(childItem);
    try
    {
        childItem->m_itemListParent.push_back(this);
    }
    catch (...)
    {
        // 第二次 push 失败：回滚父项侧的强引用，保持双向关系一致
        m_itemListChild.pop_back();
        throw;
    }
    return true;
}

bool EItem::insertChild(int index, const EItemPtr& childItemRef)
{
    EItem* child = childItemRef.get();
    if (child == nullptr)
    {
        return false;
    }
    if (child == this || isChildOf(childItemRef))
    {
        ELOG_ERROR("EItem: cannot insert an ancestor (or self) as a child");
        return false;
    }
    // 重复挂载检查: 直接扫描子项列表
    for (const EItemPtr& existing : m_itemListChild)
    {
        if (existing.get() == child)
        {
            ELOG_ERROR("Item \"%s\" is already a child of Item \"%s\", ignoring call",
                    child->getName().data(), this->getName().data());
            return false;
        }
    }

    EItemList& _children = m_itemListChild;
    const int size = static_cast<int>(_children.size());
    int insertIndex = index;
    if (insertIndex < 0)
    {
        insertIndex = 0;
    }
    else if (insertIndex > size)
    {
        insertIndex = size;
    }

    _children.insert(_children.begin() + insertIndex, child);
    try
    {
        child->m_itemListParent.push_back(this);
    }
    catch (...)
    {
        // 第二次插入失败：回滚子项列表，保持双向关系一致
        _children.erase(_children.begin() + insertIndex);
        throw;
    }
    return true;
}

bool EItem::removeChild(int index, int numChildrenToRemove)
{
    const int childCount = static_cast<int>(m_itemListChild.size());
    if (index < 0 || numChildrenToRemove <= 0 || index >= childCount)
    {
        return false;
    }
    // 先与剩余数量比较，避免 index + numChildrenToRemove 的有符号溢出
    const int remaining = childCount - index;
    const int count = (numChildrenToRemove > remaining) ? remaining : numChildrenToRemove;
    const int endOfRemoveRange = index + count;

    // 解除子项回指本项的父指针(同类可直接访问私有成员)
    for(int i = index; i < endOfRemoveRange; i++)
    {
        EItem* child = m_itemListChild[i].get();
        if (child == nullptr)
        {
            continue;
        }
        std::vector<EItem*>& parents = child->m_itemListParent;
        parents.erase(std::remove(parents.begin(), parents.end(), this), parents.end());
    }

    m_itemListChild.erase(m_itemListChild.begin()+index, m_itemListChild.begin()+endOfRemoveRange);
    return true;
}

bool EItem::removeChild(const EItemPtr& childItem)
{
    const int index = getChildIndex(childItem);
    return index >= 0 && removeChild(index, 1);
}

bool EItem::removeChildrenAll()
{
    if (m_itemListChild.empty())
    {
        return false;
    }
    // 先解除各子项回指本项的父指针，再清空列表
    // (clear 释放强引用，子项无其他引用时随之析构)
    for (size_t i = 0; i < m_itemListChild.size(); ++i)
    {
        EItem* child = m_itemListChild[i].get();
        if (child == nullptr)
        {
            continue;
        }
        std::vector<EItem*>& parents = child->m_itemListParent;
        parents.erase(std::remove(parents.begin(), parents.end(), this), parents.end());
    }
    m_itemListChild.clear();
    return true;
}

EItemPtr EItem::getChild(int index)
{
    if (index >= 0 && index < static_cast<int>(m_itemListChild.size()))
    {
        return m_itemListChild[index];
    }
    return EItemPtr();
}

size_t EItem::getChildCount()
{
    return m_itemListChild.size();
}

int EItem::getChildIndex(const EItemPtr& childItem)
{
    const EItem* target = childItem.get();
    for (size_t i = 0; i < m_itemListChild.size(); ++i)
    {
        if (m_itemListChild[i].get() == target)
        {
            return static_cast<int>(i);
        }
    }
    return -1; // node not found.
}

EItem* EItem::getParent(int index)
{
    if (index >= 0 && index < static_cast<int>(m_itemListParent.size()))
    {
        return m_itemListParent[index];
    }
    return nullptr;
}

size_t EItem::getParentCount()
{
    return m_itemListParent.size();
}

int EItem::getParentIndex(const EItemPtr& parentItem)
{
    const EItem* target = parentItem.get();
    for (size_t i = 0; i < m_itemListParent.size(); ++i)
    {
        if (m_itemListParent[i] == target)
        {
            return static_cast<int>(i);
        }
    }
    return -1; // node not found.
}

bool EItem::isParentOf(const EItemPtr& childItem) const
{
    const EItem* node = childItem.get();
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

bool EItem::isChildOf(const EItemPtr& parentItem) const
{
    const EItem* target = parentItem.get();
    if (target == nullptr)
    {
        return false;
    }
    std::vector<const EItem*> stack;
    std::unordered_set<const EItem*> visited;
    stack.push_back(this);
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
            if (parent == target)
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

void EItem::setAttached(intptr_t ptr)
{
    m_attached = ptr;
}

intptr_t EItem::getAttached()
{
    return m_attached;
}

EItemMetaPtr EItem::getOrCreatMeta()
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

    return m_itemMeta;
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
