#include "test_remove_range.h"
#include "test_check.h"

#include <climits>

#include <Item/EItem.h>

void testRemoveRange()
{
    ESmartPtr<EItem> parent(new EItem());
    ESmartPtr<EItem> a(new EItem());
    ESmartPtr<EItem> b(new EItem());
    ESmartPtr<EItem> c(new EItem());
    CHECK(parent->addChild(a));
    CHECK(parent->addChild(b));
    CHECK(parent->addChild(c));

    CHECK(!parent->removeChild(0, -1));     // 负数：不得 UB
    CHECK(!parent->removeChild(0, 0));      // 0 个：返回 false
    CHECK(!parent->removeChild(5, 1));      // 越界
    CHECK(parent->getChildCount() == 3);

    CHECK(parent->removeChild(1, INT_MAX)); // 极值钳制：删除 b、c
    CHECK(parent->getChildCount() == 1);
    CHECK(parent->getChild(0) == a.get());
    CHECK(b->getParentCount() == 0);
    CHECK(c->getParentCount() == 0);

    CHECK(parent->removeChild(0, 1));       // 正常删除
    CHECK(parent->getChildCount() == 0);
    CHECK(a->getParentCount() == 0);

    // 多父场景: 父子关系统一由父项 removeChild 维护
    ESmartPtr<EItem> p1(new EItem());
    ESmartPtr<EItem> p2(new EItem());
    ESmartPtr<EItem> sharedChild(new EItem());
    CHECK(p1->addChild(sharedChild));
    CHECK(p2->addChild(sharedChild));
    CHECK(sharedChild->getParentCount() == 2);
    CHECK(p1->removeChild(sharedChild));
    CHECK(p1->getChildCount() == 0);
    CHECK(sharedChild->getParentCount() == 1);
    CHECK(p2->removeChild(sharedChild));
    CHECK(p2->getChildCount() == 0);
    CHECK(sharedChild->getParentCount() == 0);
}
