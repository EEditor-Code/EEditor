// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2026 nwpu052009 <ljwangcumtb@163.com>

#ifndef E_ITEM_H
#define E_ITEM_H

#include <SmartPtr/ESmartPtr.h>
#include <DefineLibCommon.h>
#include <ECommonDefines.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <Item/EItemMeta.h>

/**对象树基类: 保存名称、类型、父子关系与元信息, 是树/属性/序列化等功能的共同数据基础。
 * 扩展功能通过继承本类并组合包装类实现, 不应直接修改本类。*/
class EItem;
/**EItem 的引用计数智能指针。*/
typedef ESmartPtr<EItem> EItemPtr;
/**EItemPtr 列表(子项列表等容器使用)。*/
typedef std::vector<EItemPtr> EItemList;
/**创建 EItemMeta 的回调: 由派生类或使用方通过 setFunCreateMeta 注册。*/
typedef EItemMetaPtr (*Fun_createMeta)(EItem*);

// class COMMON_DLL EItem : public std::enable_shared_from_this<EItem>
class COMMON_DLL EItem : public Referenced
{
public:
    /**构造一个空项: 无名称、无父、无子、无 meta。*/
    EItem();
    /**虚析构: 保证经基类指针销毁派生项时能正确析构。*/
    virtual ~EItem();

    /**禁止拷贝构造: 项由智能指针管理, 不支持值语义复制。*/
    EItem(const EItem&) = delete;
    /**禁止拷贝赋值。*/
    EItem& operator=(const EItem&) = delete;

    /**设置项名称(C 字符串重载)。*/
    void setName(const char* strName);
    /**设置项名称(EString 重载)。*/
    void setName(const EString& strName);
    /**获取项名称(见 m_name)。*/
    EString getName();

    /**获取类型名(由具体类在构造时写入, 外部不可修改)。*/
    EString getType();

    /**获取项 ID(见 m_id): 本项的唯一身份标识, 构造时自动分配, 生命周期内保持不变。
     *  外部(UI/RPC/管理器)引用一律使用 id, 不要依赖对象地址; 0 表示无效。*/
    int getId() const { return m_id; }

    /**设置项的文本描述(纯内部文本, 不参与 UI 显示与序列化)。*/
    void setDescription(const char* strName);
    /**获取项的文本描述。*/
    EString getDescription();

    /****添加相关函数******/
    /**添加子项(唯一公开入口): 校验空指针、自引用、重复挂载与父子环。
     *  所有权: 成功后父项持有 childItem 的一个强引用; 对象须由 ESmartPtr 管理，
     *  调用方不得再手动 delete(传栈对象/成员对象会在析构时 delete 非堆内存)。*/
    bool addChild(const EItemPtr& childItem);

    /**在指定位置插入子项: 新子项插入到原 index 位置节点之前; 越界时追加/钳制到首尾。
     *  所有权与 addChild 相同: 成功后父项持有 childItem 的一个强引用。*/
    bool insertChild(int index, const EItemPtr& childItem);

    /****移除相关函数******/
    /**按索引移除子项, 可一次移除连续 numChildrenToRemove 个; 成功返回 true。*/
    bool removeChild(int index, int numChildrenToRemove=1);
    /**removeChild 的智能指针重载: 按指针查索引后走索引版本。*/
    bool removeChild( const EItemPtr& childItem );

    /**移除全部子项(同时解除各子项对本项的父引用); 无子项时返回 false。*/
    bool removeChildrenAll();

    /****查找相关函数******/
    /**按索引获取直接子项(返回强引用, 持有期间子项不会被释放); 越界返回空 EItemPtr。*/
    EItemPtr getChild(int index);

    /**获取直接子项数量。*/
    size_t getChildCount();

    /**按智能指针获取子项索引; 未找到返回 -1。*/
    int getChildIndex(const EItemPtr& childItem);

    /**按索引获取父项(多父结构; 返回借用指针, 仅在父子关系存在时有效, 不要长期保存); 越界返回 nullptr。*/
    EItem* getParent(int index);

    /**获取父项数量(多父结构)。*/
    size_t getParentCount();

    /**按智能指针获取父项索引; 未找到返回 -1。*/
    int getParentIndex(const EItemPtr& parentItem);

    /**判断本项是否为 childItem 的父级(祖先): 沿 childItem 的父链向上查找, 不限于直接父项。*/
    bool isParentOf(const EItemPtr& childItem) const;
    /**判断本项是否为 parentItem 的子级(后代): 沿本项父链向上查找。*/
    bool isChildOf(const EItemPtr& parentItem) const;

    /**挂接一个外部对象指针(如 UI 树项缓存); 仅保存指针, 不管理其生命周期。*/
    virtual void setAttached(intptr_t ptr);
    /**返回先前挂接的外部对象指针; 未挂接时为 0。*/
    virtual intptr_t getAttached();

    /**获取本项的 meta(返回强引用); 尚未创建时用 m_fun_createMeta 懒创建(无回调则创建默认 EItemMeta)。*/
    virtual EItemMetaPtr getOrCreatMeta();

    /**直接获取 meta(可能为 nullptr, 不会触发创建)。*/
    EItemMetaPtr getMeta() const;
    /**直接设置 meta。*/
    void setMeta(EItemMetaPtr meta);

    /**设置创建 meta 的回调(由派生类或使用方注册)。*/
    void setFunCreateMeta(Fun_createMeta fun);

    /**返回调试字符串: 名称、类型、子项数量、父项数量。*/
    virtual EString getStrForDebug();

protected:
    /*!
        \brief 唯一 ID，构造时自动分配，对象生命周期内保持不变。
        \~english Unique identifier, assigned automatically at construction and stable for the object's lifetime.
        \nameCN ID
        \editable false
    */
    int m_id = 0;

    /**本项的元信息(属性/函数描述); 由 getOrCreatMeta() 惰性创建。*/
    EItemMetaPtr m_itemMeta;

    /**父项列表(多父结构; 顺序即父链顺序)。*/
    std::vector<EItem*> m_itemListParent;
    /**子项列表(顺序即树中的显示顺序)。*/
    std::vector<EItemPtr> m_itemListChild;

    /**创建 meta 的回调(经 setFunCreateMeta 注册)。*/
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
    /* ==== 以下两个成员刻意放 private ====
     * 它们是内部状态, 不应参与反射/属性面板(完整平台的反射只收集非 private 成员):
     *   m_attached    外部挂载指针, 供 UI 树项缓存使用;
     *   m_description 项的纯内部文本描述。
     * 对外读写一律经 public 访问器:
     *   setAttached/getAttached、setDescription/getDescription。*/
    intptr_t m_attached = 0;            ///< 外部挂载指针(如 UI 树项缓存)
    EString m_description;              ///< 项的文本描述(经 setDescription/getDescription 读写)

};

#endif // EITEM_H
