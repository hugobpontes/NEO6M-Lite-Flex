#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Neo6m_MetaTests)
{
  RUN_TEST_CASE(Neo6m_MetaTests, GetMsReturnsDataAsSet);
  RUN_TEST_CASE(Neo6m_MetaTests, FullByteReadCopiesAndReturnsSetDataLength);
  RUN_TEST_CASE(Neo6m_MetaTests, IncompleteByteReadCopiesAndReturnsSetDataLength);
}

TEST_GROUP_RUNNER(Neo6m_CopyFromBufferToRingBuffer)
{
  RUN_TEST_CASE(Neo6m_IOReadIntoToRingBuffer, DataIsWrittenOnEmptyBuffer);
}

