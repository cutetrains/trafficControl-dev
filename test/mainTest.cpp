#include "tst_tcNetworkTests.h"
#include "tst_tcStationTests.h"
#include "tst_tcTrainTests.h"
#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
