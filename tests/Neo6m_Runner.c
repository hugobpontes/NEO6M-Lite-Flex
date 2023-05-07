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

TEST_GROUP_RUNNER(Neo6m_GetBytesUntilSequenceEnd)
{
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, Finds1Byte);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, FindsString);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, FindsStringAtStart);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, FindsStringAtEnd);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, UnexistentStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, NullStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, TooLargeStringReturnsFFFFFFFF);
  RUN_TEST_CASE(Neo6m_GetBytesUntilSequenceEnd, ReadPointerDoesntMove);
}

TEST_GROUP_RUNNER(Neo6m_GetCharBeforeSequence)
{
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsCharBeforeSequence);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfNotFound);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfTooLargeString);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfNullString);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfEmptyBuffer);
  RUN_TEST_CASE(Neo6m_GetCharBeforeSequence, ReturnsXIfSequenceIsNext);
}

TEST_GROUP_RUNNER(Neo6m_GetFloatUntilSequence)
{
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsFloatUntilSequence);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfSequenceIsNext);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfSequenceNotFound);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfSequenceNull);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfSequenceTooLong);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfBytesUntilStringTooLong);
  RUN_TEST_CASE(Neo6m_GetFloatUntilSequence, ReturnsErrorIfFloatStringContainsNonNumeric);
}
TEST_GROUP_RUNNER(Neo6m_GetIntUntilSequence)
{
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsIntUntilSequence);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfSequenceIsNext);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfSequenceNotFound);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfSequenceNull);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfSequenceTooLong);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfBytesUntilStringTooLong);
  RUN_TEST_CASE(Neo6m_GetIntUntilSequence, ReturnsErrorIfIntStringContainsNonNumeric);
}
TEST_GROUP_RUNNER(Neo6m_GetNextBytesAsInt)
{
  RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, ReturnsNextBytesAsInt);
  RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, ReturnsErrorIfNonNumeric);
  RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, ReturnsErrorIfBufferEmpty);
}

TEST_GROUP_RUNNER(Neo6m_FillInTrackingNeo6mMsgStruct)
{
	RUN_TEST_CASE(Neo6m_FillInTrackingNeo6mMsgStruct, FillsAsExpectedFromTrackingData);
	/*RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, FillsAsExpectedFromNonTrackingData);
	RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, FillsAsExpectedFromTrackingDataWith1SvMsg);
	RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, ReturnsEmptyIfGPRMCNotFound);
	RUN_TEST_CASE(Neo6m_GetNextBytesAsInt, ReturnsPartiallyFilledIfBufferEmpties)*/
}


