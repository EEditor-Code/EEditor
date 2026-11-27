#include "test_matrix_scale.h"
#include "test_check.h"

#include <Variant/EMatrix4x4.h>

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
