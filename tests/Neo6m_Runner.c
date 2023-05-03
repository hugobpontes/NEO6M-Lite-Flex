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
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, DataIsWrittenOnEmptyBuffer);
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, DataIsAppendedOnPartiallyFullBuffer);
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, DataIsAppendedCircularly);
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, NullPointerIsReported);
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, OverflowIsReported);
  RUN_TEST_CASE(Neo6m_IOReadIntoRingBuffer, IOErrorIsReported);
}

TEST_GROUP_RUNNER(Neo6m_GetBytesUntilSequence)
{
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, Finds1Byte);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, FindsString);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, FindsStringAtStart);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, FindsStringAtEnd);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, UnexistentStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, NullStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, TooLargeStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequence, ReadPointerDoesntMove);
}

TEST_GROUP_RUNNER(Neo6m_GetCharBeforeSequence)
{
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsCharBeforeSequenceAndReadPtrAdvances);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfNotFound);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfTooLargeString);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfNullString);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfEmptyBuffer);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfSequenceIsNext);
}
