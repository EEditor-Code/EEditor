#include "test_check.h"
#include "test_smart_ptr.h"
#include "test_eitem_tree.h"
#include "test_parent_destruction.h"
#include "test_remove_range.h"
#include "test_matrix_scale.h"
#include "test_string_util.h"
#include "test_default_init.h"

#include <cstdio>
#include <type_traits>

#include <Item/EItem.h>
#include <Item/EItemMeta.h>
#include <Item/EItemMetaFunction.h>

int g_failures = 0;

static_assert(!std::is_copy_constructible<EItem>::value, "EItem must not be copyable");
static_assert(!std::is_copy_assignable<EItem>::value, "EItem must not be copy-assignable");
static_assert(!std::is_copy_constructible<EItemMeta>::value, "EItemMeta must not be copyable");
static_assert(!std::is_copy_assignable<EItemMeta>::value, "EItemMeta must not be copy-assignable");
static_assert(!std::is_copy_constructible<EItemMetaFunction>::value,
              "EItemMetaFunction must not be copyable");
static_assert(!std::is_copy_assignable<EItemMetaFunction>::value,
              "EItemMetaFunction must not be copy-assignable");

int main()
{
    testSmartPtrDynamicCast();
    testEItemTree();
    testParentDestruction();
    testRemoveRange();
    testMatrixScale();
    testStringUtil();
    testDefaultInit();

    if (g_failures == 0)
    {
        std::printf("ALL TESTS PASSED\n");
        return 0;
    }

    std::printf("%d TEST(S) FAILED\n", g_failures);
    return 1;
}
