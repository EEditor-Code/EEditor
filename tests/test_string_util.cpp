#include "test_string_util.h"
#include "test_check.h"

#include <Variant/EString.h>

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
