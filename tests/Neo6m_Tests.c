#include "Neo6m_MockIO.h"

#include "unity.h"
#include "unity_fixture.h"

#include "Neo6mLiteFlex.h"

#include "Neo6mPrivates.h"

static Neo6mLiteFlex_t Neo6m;
static lwrb_t* pRingBuf;
static uint8_t* ByteArray;
static Neo6mLiteFlexStatus_t Status;
static uint8_t ExpectedBuf[NEO6M_BATCH_BUFFER_SIZE];
static uint8_t ActualBuf[NEO6M_BATCH_BUFFER_SIZE];
static size_t OldReadPointer;
static size_t NewReadPointer;


/*ZZ Replaces 0x0D and 0x0A (Line Feed+Carriage Return) which will be ignored by the parser*/
static char Neo6mTrackingDataSet[] =
		"$GPVTG,T,,M,0.196,N,0.364,K,A*2CZZ"
		"$GPGGA,140721.00,2769.50673,N,00321.52120,W,1,05,5.79,130.4,M,48.3,M,,*4AZZ"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CZZ"
		"$GPGSV,2,1,06,02,30,050,44,08,,,28,10,48,093,40,18,06,050,43*44ZZ"
		"$GPGSV,2,2,06,23,30,053,47,27,69,030,38*7AZZ"
		"$GPGLL,2769.50673,N,00321.52120,W,140721.00,A,A*78ZZ"
		"$GPRMC,140722.00,A,2769.50696,N,00321.52135,W,0.219,,290423,,,A*69ZZ"
		"$GPVTG,,T,,M,0.219,N,0.405,K,A*28ZZ"
		"$GPGGA,140722.00,2769.50696,N,00321.52135,W,1,05,5.79,130.5,M,48.3,M,,*47ZZ"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CZZ"
		"$GPGSV,2,1,06,02,30,050,44,08,,,28,10,48,093,40,18,06,050,43*44ZZ"
		"$GPGSV,2,2,06,23,30,053,47,27,69,030,38*7AZZ"
		"$GPGLL,2769.50696,N,00321.52135,W,140722.00,A,A*74ZZ"
		"$GPRMC,140722.00,A,2769.50696,N,0091";

static char Neo6mNonTrackingDataSet[] =
		"VTG,,,,,,,,,N*30ZZ"
		"$GPGGA,,,,,,0,00,99.99,,,,,"
		",*48ZZ"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30ZZ"
		"$GPGSV,1,1,00*79ZZ"
		"$GPGLL,,,,,,V,N*64ZZ"
		"$GPRMC,,V,,,,,,,,,,N*53ZZ"
		"$GPVTG,,,,,,,,,N*30ZZ"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48ZZ"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30ZZ"
		"$GPGSV,1,1,00*79ZZ"
		"$GPGLL,,,,,,V,N*64ZZ"
		"$GPRMC,,V,,,,,,,,,,N*53ZZ"
		"$GPVTG,,,,,,,,,N*30ZZ"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48ZZ"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30ZZ"
		"$GPGSV,1,1,00*79ZZ"
		"$GPGLL,,,,,,V,N*64ZZ"
		"$GPRMC,,V,,,,,,,,,,N*53ZZ"
		"$GPVTG,,,,,,,,,N*30ZZ"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48ZZ"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30ZZ"
		"$GPGSV,1,1,00*79ZZ"
		"$GPGLL,,,,,,V,N*64ZZ"
		"$GPRMC,,V,,,,,,,,,,N*53ZZ"
		"$GPVTG,,,,,,,,,N*30ZZ"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48ZZ"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30ZZ"
		"$GPGSV";
static char TooLargeFloatDataSet[] =
		"$GPVTG,T,,M,0.196293892328372733,";
static char BadFloatDataSet[] =
		"$GPVTG,T,,M,0.1962.38929,";

static void TestReadPointerAdvanced(uint32_t Advance)
{
	NewReadPointer = pRingBuf->r;
	TEST_ASSERT_EQUAL(Advance,NewReadPointer-OldReadPointer);
}

static void SkipAndUpdateTestReadPtr(uint32_t skip)
{
	lwrb_skip(pRingBuf,skip);
	OldReadPointer = pRingBuf->r;
}


TEST_GROUP(Neo6m_MetaTests);

TEST_SETUP(Neo6m_MetaTests)
{
	MockNeo6m_Create(20);
}

TEST_TEAR_DOWN(Neo6m_MetaTests)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_MetaTests,GetMsReturnsDataAsSet)
{
	MockNeo6m_ExpectGetMsAndReturn(50);
	uint32_t A;

	A = MockNeo6m_GetMs();

	TEST_ASSERT_EQUAL(A,50);
	/*Also tested that it fails if no get ms is called */
}

TEST(Neo6m_MetaTests,FullByteReadCopiesAndReturnsSetDataLength)
{
	uint8_t DataPtr[5];
	uint8_t SrcPtr[5] = {1,2,3,4,5};

	MockNeo6m_ExpectReadAndReturn(5, SrcPtr, 5);
	size_t A;

	A = MockNeo6m_Read(DataPtr, 5);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(SrcPtr,DataPtr,5);
	TEST_ASSERT_EQUAL(5,A);
	/*Also tested that it fails if no read is called */
}

TEST(Neo6m_MetaTests,IncompleteByteReadCopiesAndReturnsSetDataLength)
{
	uint8_t DataPtr[5];
	uint8_t SrcPtr[3] = {1,2,3};

	MockNeo6m_ExpectReadAndReturn(5, SrcPtr, 3);
	size_t A;

	A = MockNeo6m_Read(DataPtr, 5);

	TEST_ASSERT_EQUAL_CHAR_ARRAY(SrcPtr,DataPtr,3);
	TEST_ASSERT_NOT_EQUAL(SrcPtr[4],DataPtr[4]);
	TEST_ASSERT_EQUAL(3,A);
	/*Also tested that it fails if no read is called */
}

/*-----------------------------------------------------------------------------------------------------------------*/
TEST_GROUP(Neo6m_IOReadIntoRingBuffer);

TEST_SETUP(Neo6m_IOReadIntoRingBuffer)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
	Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);

	pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);
	ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);
}

TEST_TEAR_DOWN(Neo6m_IOReadIntoRingBuffer)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

/*Tests that reading into an empty ring buffer calls read IO as expected, and leaves the buffer in the
 * expected state containing the expected data */
TEST(Neo6m_IOReadIntoRingBuffer,DataIsWrittenOnEmptyBuffer)
{

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);

	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	memcpy(ExpectedBuf,Neo6mTrackingDataSet,NEO6M_BATCH_SIZE);
	lwrb_read(pRingBuf, ActualBuf, NEO6M_BATCH_SIZE);

	TEST_ASSERT_EQUAL(NEO6M_SUCCESS,Status);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(ExpectedBuf,ActualBuf,NEO6M_BATCH_SIZE);
}
/*Tests that reading into an ring buffer pre-filled with 10 bytes calls read IO as expected, and leaves the buffer in the
 * expected state (write pointer in accumulated size position and containing the expected data (the first 10 bytes
 * of the data set and then its full size) */
TEST(Neo6m_IOReadIntoRingBuffer,DataIsAppendedOnPartiallyFullBuffer)
{
	size_t DummyFirstCallSize = 10;
	MockNeo6m_ExpectReadAndReturn(DummyFirstCallSize, Neo6mTrackingDataSet, DummyFirstCallSize);
	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);

	IOReadIntoRingBuffer(Neo6m,DummyFirstCallSize);
	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	memcpy(ExpectedBuf,Neo6mTrackingDataSet,10);
	memcpy(ExpectedBuf+10,Neo6mTrackingDataSet,NEO6M_BATCH_SIZE);
	lwrb_read(pRingBuf, ActualBuf, NEO6M_BATCH_SIZE+DummyFirstCallSize);

	TEST_ASSERT_EQUAL(NEO6M_SUCCESS,Status);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(ExpectedBuf,ActualBuf,NEO6M_BATCH_SIZE+DummyFirstCallSize);
}

/*Tests that if the reading into the circular buffer needs to circle back to the beginning of the byte buffer, two separates reads are made
 * and that the read data is as expected.
 */
TEST(Neo6m_IOReadIntoRingBuffer,DataIsAppendedCircularly)
{
	/*Make buffer start at the middle of the allocated memory*/
	lwrb_advance(pRingBuf,NEO6M_BATCH_SIZE);
	lwrb_skip(pRingBuf,NEO6M_BATCH_SIZE);

	MockNeo6m_ExpectReadAndReturn(NEO6M_BUFFER_SAFETY_MARGIN, Neo6mTrackingDataSet, NEO6M_BUFFER_SAFETY_MARGIN);
	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE-NEO6M_BUFFER_SAFETY_MARGIN, Neo6mTrackingDataSet+NEO6M_BUFFER_SAFETY_MARGIN, NEO6M_BATCH_SIZE-NEO6M_BUFFER_SAFETY_MARGIN);

	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	memcpy(ExpectedBuf,Neo6mTrackingDataSet,NEO6M_BATCH_SIZE);
	lwrb_read(pRingBuf, ActualBuf, NEO6M_BATCH_SIZE);

	TEST_ASSERT_EQUAL(NEO6M_SUCCESS,Status);
	TEST_ASSERT_EQUAL_CHAR_ARRAY(ExpectedBuf,ActualBuf,NEO6M_BATCH_SIZE);

}

TEST(Neo6m_IOReadIntoRingBuffer,NullPointerIsReported)
{
	Neo6mLiteFlex_SetIORead(Neo6m, NULL);

	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	TEST_ASSERT_EQUAL(NEO6M_NULL_PTR,Status);
}

TEST(Neo6m_IOReadIntoRingBuffer,OverflowIsReported)
{
	lwrb_advance(pRingBuf, NEO6M_BATCH_SIZE);

	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	TEST_ASSERT_EQUAL(NEO6M_BUFFER_OVERFLOW,Status);
}

TEST(Neo6m_IOReadIntoRingBuffer,IOErrorIsReported)
{

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, 10);

	Status = IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	TEST_ASSERT_EQUAL(NEO6M_IO_ERROR,Status);
}

/*-----------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(Neo6m_GetBytesUntilSequenceEnd);

TEST_SETUP(Neo6m_GetBytesUntilSequenceEnd)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
	Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);

	pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);
	ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_GetBytesUntilSequenceEnd)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_GetBytesUntilSequenceEnd,Finds1Byte)
{
	TEST_ASSERT_EQUAL_UINT32(7,GetBytesUntilSequenceEnd(pRingBuf,","));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,FindsString)
{
	lwrb_skip(pRingBuf,10);
	TEST_ASSERT_EQUAL_UINT32(31,GetBytesUntilSequenceEnd(pRingBuf,"GPGGA,"));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,FindsStringAtStart)
{
	TEST_ASSERT_EQUAL_UINT32(7,GetBytesUntilSequenceEnd(pRingBuf,"$GPVTG,"));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,FindsStringAtEnd)
{
	lwrb_skip(pRingBuf,740);
	char mychar;
	TEST_ASSERT_EQUAL_UINT32(9,GetBytesUntilSequenceEnd(pRingBuf,",0091"));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,UnexistentStringReturnsFFFFFFFF)
{
	TEST_ASSERT_EQUAL_UINT32(SEQUENCE_NOT_FOUND,GetBytesUntilSequenceEnd(pRingBuf,"Hello"));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,NullStringReturnsFFFFFFFF)
{
	TEST_ASSERT_EQUAL_UINT32(SEQUENCE_NOT_FOUND,GetBytesUntilSequenceEnd(pRingBuf,""));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,TooLargeStringReturnsFFFFFFFF)
{
	TEST_ASSERT_EQUAL_UINT32(SEQUENCE_NOT_FOUND,GetBytesUntilSequenceEnd(pRingBuf,"$GPVTG,,T,,M,0.196"));
}

TEST(Neo6m_GetBytesUntilSequenceEnd,ReadPointerDoesntMove)
{
	GetBytesUntilSequenceEnd(pRingBuf,"GPGGA");
	TestReadPointerAdvanced(0);
}

/*-----------------------------------------------------------------------------------------------------------------*/

TEST_GROUP(Neo6m_GetCharBeforeSequence);

TEST_SETUP(Neo6m_GetCharBeforeSequence)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
	Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);

	pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);
	ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	lwrb_skip(pRingBuf,7); /*Skip until first piece of data ('T')*/

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_GetCharBeforeSequence)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsCharBeforeSequence)
{
	TEST_ASSERT_EQUAL_CHAR('4',GetCharBeforeSequence(pRingBuf,",K"));
	TestReadPointerAdvanced(20);
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsXIfNotFound)
{
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,"?"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsXIfTooLargeString)
{
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,"$GPVTG,,T,,M,0.196"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsXIfNullString)
{
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,""));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsXIfEmptyBuffer)
{
	SkipAndUpdateTestReadPtr(750);
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,","));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetCharBeforeSequence,ReturnsXIfSequenceIsNext)
{
	SkipAndUpdateTestReadPtr(2);
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,","));
	TestReadPointerAdvanced(1);
}

/*-----------------------------------------------------------------------------------------------------------------*/
TEST_GROUP(Neo6m_GetFloatUntilSequence);

TEST_SETUP(Neo6m_GetFloatUntilSequence)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
	Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);

	pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);
	ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_GetFloatUntilSequence)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsFloatUntilSequence)
{
	SkipAndUpdateTestReadPtr(12);
	TEST_ASSERT_EQUAL_FLOAT(0.196,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(6);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfSequenceIsNext)
{
	SkipAndUpdateTestReadPtr(11);
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(1);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfSequenceNotFound)
{
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,"Hello"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfSequenceNull)
{
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,""));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfSequenceTooLong)
{
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,",K,A*28ZZ$GPGGA"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfBytesUntilStringTooLong)
{
	/*Empty buffer and fill in specific data, for this test only */
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,TooLargeFloatDataSet,sizeof(TooLargeFloatDataSet));

	SkipAndUpdateTestReadPtr(12);
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(21);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfFloatStringContainsNonNumeric)
{
	/*Empty buffer and fill in specific data, for this test only */
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,BadFloatDataSet,sizeof(BadFloatDataSet));

	SkipAndUpdateTestReadPtr(12);
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(13);
}
