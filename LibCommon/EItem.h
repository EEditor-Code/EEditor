// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_ITEM_H
#define E_ITEM_H

#include "SmartPtr/ESmartPtr.h"
#include <DefineLibCommon.h>
#include <DefineCommon.h>
#include <cstddef>
#include <cstdint>
#include <EItemMeta.h>

/**The base class that stores all useful information for ui.
 * All other extended functions should inherit this class and build a wrap class type.
 * */
class EItem;
typedef ESmartPtr<EItem> EItemPtr;
typedef std::vector<EItemPtr> EItemList;
typedef EItemMetaPtr (*Fun_createMeta)(EItem*);

// class COMMON_DLL EItem : public std::enable_shared_from_this<EItem>
class COMMON_DLL EItem : public Referenced
{
public:
    EItem();
    virtual ~EItem();

    EItem(const EItem&) = delete;
    EItem& operator=(const EItem&) = delete;

    void setName(const char* strName);
    void setName(const EString& strName);
    EString getName();

    EString getType();

    /*A int tag value for item manager in outside program. This value is just the conversion of this pointer to int*/
    long getTag();

    int getId() const { return m_id; }

    /*!
        \brief 唯一 ID，由 EItemManager 自动分配。
        \~english Unique identifier, auto-assigned by EItemManager.
        \nameCN ID
        \editable false
    */
    int m_id = 0;

    void setIfShownInUI(bool boolValue);
    bool getIfShownInUI();

    /**UI 可删性: false ⇒ 经 UI 的删除入口(工具栏/右键菜单/RPC 的 delete_items)跳过本项。
     *  ★ 刻意**只约束"经 UI 的删除"**, 不是不变量 —— 内部代码仍可直接 removeChild
     *    (与"子项白名单"同样的定位: 界面约束而非强制, 见 ESensorRoot.h 的同类说明)。
     *    判定点: EUITree::uiDeleteItems / uiDeleteSubTreeItems 的过滤步骤
     *    —— 必须在建 ECommand 快照**之前**, 否则撤销会把本项复活。*/
    void setUIDeletable(bool boolValue);
    bool isUIDeletable() const;

    void setDescription(const char* strName);
    EString getDescription();

    /****Adding releated functions******/
    /**add child to this*/
    virtual bool addChild(EItem* childItem);
    bool addChild(const EItemPtr& childItem) { return addChild(childItem.get()); }

    /** Insert child at specific location.
     * The new child is inserted into the child list
     * before the node at the specified index.
     */
    virtual bool insertChild(int index, EItem* child );
    bool insertChild(int index, const EItemPtr& childItem) { return insertChild(index, childItem.get()); }

    /**临时挂载(序列化收集等临时容器用): 纯挂载, 不检查 model 归属、不改变 m_model。
     * 调用方必须保证不破坏 model 语义(如仅用于 ECommand 序列化收集)。*/
    bool attachChild(EItem* child);
    bool attachChild(const EItemPtr& childItem) { return attachChild(childItem.get()); }

    /****Finding releated functions******/
    /**get child item from this*/
    virtual EItem* getChild(int index);
    /**get child item from this by type name*/
    virtual EItem* getChildByTypeName(const EString& typeName);

    virtual EItem* getParent(int index);

    /**get child item index by ptr, return -1 if not found*/
    virtual int getChildIndex(EItem* childItem);
    int getChildIndex(const EItemPtr& childItem) { return getChildIndex(childItem.get()); }

    /**get parent item index by ptr, return -1 if not found*/
    virtual int getParentIndex(EItem* parentItem);
    int getParentIndex(const EItemPtr& parentItem) { return getParentIndex(parentItem.get()); }

    virtual size_t getParentCount();

    /**get children count*/
    size_t getChildCount();
    /** Return true if child is contained within. */
    bool containsChild( const EItem* child );
    template<class T> bool containsChild( const ESmartPtr<T>& child ) { return containsChild(child.get()); }

    /****removing releated functions******/
    /**remove a child by index*/
    virtual bool removeChild(int index, int numChildrenToRemove=1);
    /**remove a child by ptr*/
    virtual bool removeChild(EItem* childItem);
    template<class T> bool removeChild( const ESmartPtr<T>& childItem ) { return removeChild(childItem.get()); }

    /**remove a child by index*/
    virtual bool removeParent(int index, int numParentToRemove=1);

    /**remove a parent by ptr*/
    virtual bool removeParent(EItem* parentItem);
    template<class T> bool removeParent( const ESmartPtr<T>& parentItem ) { return removeParent(parentItem.get()); }

    /**remove all children*/
    virtual bool removeChildrenAll();

    //Enable to attach some outer object for convenient
    virtual void setAttached(intptr_t ptr);
    virtual intptr_t getAttached();

    /****Changing releated functions******/
    //convenient function for setting property's ifShowUI
    void setPropertyIfShowUI(const EString& propName , bool value);
    /**隐藏/恢复某声明类的属性组(组名 = autogen 写入的 groupName, 即声明类名)。
     * 对本项及其子树生效: EUIProperty 会沿当前项父链查询。仅影响属性面板显示。*/
    void setPropertyGroupVisible(const EString& groupName, bool visible)
    {
        this->getOrCreatMeta()->setPropertyGroupVisible(groupName, visible);
    }
    //convenient function for setting property's ifSyncUIEveryFrame
    void setPropertyIfSyncUIEveryFrame(const EString& propName, bool value); 
    //convenient function for setting item's icon name
    void setPropertyIconName(const EString& iconName);
    EString getPropertyIconName();
    // virtual bool changeParent(EItem* parentNew, EItem* parentOld);
    // virtual void selectThis();

    void setIfHandledInTree(bool value);
    bool getIfHandledInTree();

    /* ==== 树结构访问 ==== */
    /**第一个父节点(多父保留; 无父返回 nullptr)*/
    EItem* parent() const;
    /**在第一个父中的行号(无父返回 -1)*/
    int row() const;

    virtual EItemMeta* getOrCreatMeta();

    // Get/Set item meta directly
    EItemMetaPtr getMeta() const;
    void setMeta(EItemMetaPtr meta);

    //called by autogen.h
    void setFunCreateMeta(Fun_createMeta fun);

    virtual EString getStrForDebug();

    /* ==== 仿真生命周期钩子(默认不参与; 子类按需 override) ====
     * 约定:
     *  - 参与者必须同时 override 钩子与 isSimulationParticipant();
     *  - onSimulationStart 保存原始状态(快照), onSimulationStop 用快照恢复;
     *  - 恢复请经属性通道写回(vsg/属性面板同步); 管理器在仿真期间已全局抑制 undo,
     *    参与者不要再自行 beginSuppressUndo/endSuppressUndo;
     *  - 主线程调用。*/
    /**是否参与仿真(参与者计数、仿真期可编辑项判据)。默认 false。*/
    virtual bool isSimulationParticipant() const;
    /**仿真开始: 保存原始状态(快照)。默认空实现。*/
    virtual void onSimulationStart();
    /**仿真暂停/恢复: 可选钩子。默认空实现。*/
    virtual void onSimulationPause();
    virtual void onSimulationResume();
    /**仿真结束: 用开始时的快照恢复原始状态。默认空实现。*/
    virtual void onSimulationStop();

protected:
    // Item meta (moved from EItemData to direct member)
    EItemMetaPtr m_itemMeta;

    // child and parent lists
    EList<EItem*> m_itemListParent;
    EList<EItemPtr> m_itemListChild;

    // Function for EItemMeta generation
    Fun_createMeta m_fun_createMeta = nullptr;

    /*!
        \brief 项的名称(树列表与 UI 显示用)。
        \~english Name of the item (shown in the tree and UI).
        \nameCN 名称
    */
    EString m_name;
    /*!
        \brief 类型名(由具体类在构造时写入, 外部不可设置)。
        \~english Type name of this class, written by the concrete class; not settable from outside.
        \nameCN 类型
        \editable false
    */
    EString m_type;

private:
    // 数据层纯插入/移除(无通知)
    bool isAncestorOf(const EItem* node) const;
    bool d_addChild(EItem* child);
    bool d_insertChild(int index, EItem* child);
    /**纯挂载(不检查 model 归属, 供 model 内部 move 复用已挂载 item)*/
    bool d_attachChild(int index, EItem* child);
    bool d_removeChild(int index, int numChildrenToRemove = 1);
    void d_clearChildren();

    /* ==== 以下五个成员刻意放 private ====
     * ExeReflection 只收集非 private 的 m_ 成员(AutogenCodeGenerator.cpp:203-207),
     * 故 private 天然不被注册成属性 —— 它们都是内部状态, 出现在属性面板里可编辑是 footgun:
     *   m_attached        被误改会破坏 EUITreeQt 的树项缓存;
     *   m_ifHandledInTree 被误改会让增量建树错乱(该显的不显、该重的不重);
     *   m_ifShownInUI     无写入方, 恒为 true;
     *   m_uiDeletable     被误改只是让某个节点能/不能被 UI 删掉, 影响面小但同样不该出现在面板上;
     *   m_description     纯内部文本(2026-10-14 由 protected 移入), 不应出现在属性面板。
     * 对外读写一律经 public 访问器:
     *   setIfShownInUI/getIfShownInUI、setIfHandledInTree/getIfHandledInTree、
     *   setAttached/getAttached、setUIDeletable/isUIDeletable、
     *   setDescription/getDescription。*/
    bool m_ifShownInUI = true;          // 是否在 UI 中显示(默认 true)
    bool m_ifHandledInTree = false;     // 树结构是否已处理(避免重复遍历)
    bool m_uiDeletable = true;          // 是否允许经 UI 删除(默认 true = 现状不变)
    intptr_t m_attached = NULL;         // 外部挂载指针(如 EUITreeQt 的 QTreeItemExtended*)
    EString m_description;              // 项的文本描述(经 setDescription/getDescription 读写)

};

#endif // EITEM_H
