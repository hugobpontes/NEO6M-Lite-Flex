#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Neo6mMetaTests)
{
  RUN_TEST_CASE(Neo6mMetaTests, GetMsReturnsDataAsSet);
  RUN_TEST_CASE(Neo6mMetaTests, FullByteReadCopiesAndReturnsSetDataLength);
}

