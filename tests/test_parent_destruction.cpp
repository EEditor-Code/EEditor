#include "test_parent_destruction.h"
#include "test_check.h"

#include <Item/EItem.h>

void testParentDestruction()
{
    ESmartPtr<EItem> parent(new EItem());
    ESmartPtr<EItem> child(new EItem());
    CHECK(parent->addChild(child));

    parent = nullptr;                        // 父项析构，子项因外部引用存活
    CHECK(child->getParentCount() == 0);     // 子项不再持有悬空父指针

    // 防环遍历不再触碰已释放的父节点
    ESmartPtr<EItem> other(new EItem());
    CHECK(child->addChild(other));
}
