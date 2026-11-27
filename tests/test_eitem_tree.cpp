#include "test_eitem_tree.h"
#include "test_check.h"

#include <Item/EItem.h>

void testEItemTree()
{
    ESmartPtr<EItem> grandparent(new EItem());
    ESmartPtr<EItem> parent(new EItem());
    ESmartPtr<EItem> child(new EItem());

    CHECK(grandparent->addChild(parent));
    CHECK(parent->addChild(child));
    CHECK(!parent->addChild(child));
    CHECK(!child->addChild(child));
    CHECK(!child->addChild(grandparent));

    CHECK(child->getParentCount() == 1);
    CHECK(parent->removeChild(child));
    CHECK(child->getParentCount() == 0);
    CHECK(!grandparent->removeChild(child));
}
