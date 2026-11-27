#include "test_default_init.h"
#include "test_check.h"

#include <Item/EItem.h>
#include <Item/EItemMeta.h>
#include <Item/EItemMetaFunction.h>
#include <Item/EItemMetaProperty.h>

void testDefaultInit()
{
    EItemMetaFunction function;
    CHECK(function.getFunType() == EItemMetaFunction::FREE);

    EItemMetaProperty property;
    CHECK(property.m_owner == 0);

    EItem first;
    EItem second;
    CHECK(first.getId() != second.getId());
    CHECK(first.getId() >= 1);
    const int firstId = first.getId();
    first.setName("renamed");
    CHECK(first.getId() == firstId);
}
