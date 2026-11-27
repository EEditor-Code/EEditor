#include "test_smart_ptr.h"
#include "test_check.h"

#include <Item/EItem.h>
#include <SmartPtr/ESmartPtr.h>

namespace
{
class DerivedItem : public EItem
{
public:
    int value = 0;
};
}

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
