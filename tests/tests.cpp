#include <cstdio>
#include <type_traits>

#include <Item/EItem.h>
#include <Item/EItemMeta.h>
#include <Item/EItemMetaFunction.h>
#include <Item/EItemMetaProperty.h>
#include <Variant/EMatrix4x4.h>
#include <Variant/EString.h>
#include <SmartPtr/ESmartPtr.h>

namespace
{
int g_failures = 0;

#define CHECK(cond)                                                          \
    do                                                                       \
    {                                                                        \
        if (!(cond))                                                         \
        {                                                                    \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

class DerivedItem : public EItem
{
public:
    int value = 0;
};

void testSmartPtrDynamicCast()
{
    ESmartPtr<DerivedItem> derived(new DerivedItem());
    derived->value = 42;

    ESmartPtr<EItem> base = derived;
    ESmartPtr<DerivedItem> back = ESmartPtr<EItem>::dynamicCast<DerivedItem>(base);

    CHECK(back.get() == derived.get());
    CHECK(back->value == 42);
    CHECK(derived->getRefCount() == 3);

    ESmartPtr<EItem> plain(new EItem());
    ESmartPtr<DerivedItem> invalid = ESmartPtr<EItem>::dynamicCast<DerivedItem>(plain);
    CHECK(invalid.get() == nullptr);
}

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
    CHECK(!child->removeParent(-1));
    CHECK(!child->removeParent(5));
    CHECK(child->getParentCount() == 1);
    CHECK(child->removeParent(0, 99));
    CHECK(child->getParentCount() == 0);
}

void testMatrixScale()
{
    EMatrix4x4 m;
    m.makeScale(EVec3(2.0, 3.0, 4.0));

    CHECK(m.data[0][0] == 2.0);
    CHECK(m.data[1][1] == 3.0);
    CHECK(m.data[2][2] == 4.0);
    CHECK(m.data[3][0] == 0.0);
    CHECK(m.data[3][1] == 0.0);
    CHECK(m.data[3][2] == 0.0);
    CHECK(m.data[3][3] == 1.0);
}

void testStringUtil()
{
    EString text = "a-b-c";
    EStringUtil::remove(text, "");
    CHECK(text == "a-b-c");

    CHECK(EStringUtil::number(1.5, "%f") == "1.500000");
    CHECK(EStringUtil::number(1.5, "%s") == "1.500000");
    CHECK(EStringUtil::number(1.5, "%n") == "1.500000");
    CHECK(EStringUtil::number(1.5, "%100000f").size() < 128);
}

void testDefaultInit()
{
    EItemMetaFunction function;
    CHECK(function.getFunType() == EItemMetaFunction::FREE);

    EItemMetaProperty property;
    CHECK(property.m_owner == 0);

    EItem first;
    EItem second;
    CHECK(first.getId() != second.getId());
}

static_assert(!std::is_copy_constructible<EItem>::value, "EItem must not be copyable");
static_assert(!std::is_copy_assignable<EItem>::value, "EItem must not be copy-assignable");
static_assert(!std::is_copy_constructible<EItemMeta>::value, "EItemMeta must not be copyable");
static_assert(!std::is_copy_assignable<EItemMeta>::value, "EItemMeta must not be copy-assignable");
static_assert(!std::is_copy_constructible<EItemMetaFunction>::value,
              "EItemMetaFunction must not be copyable");
static_assert(!std::is_copy_assignable<EItemMetaFunction>::value,
              "EItemMetaFunction must not be copy-assignable");
}

int main()
{
    testSmartPtrDynamicCast();
    testEItemTree();
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
