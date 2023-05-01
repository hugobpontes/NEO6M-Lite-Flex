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


/*XX Replaces 0x0D and 0x0A (Line Feed+Carriage Return) which will be ignored by the parser*/
static char Neo6mTrackingDataSet[] =
		"$GPVTG,,T,,M,0.196,N,0.364,K,A*2CXX"
		"$GPGGA,140721.00,2769.50673,N,00321.52120,W,1,05,5.79,130.4,M,48.3,M,,*4AXX"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CXX"
		"$GPGSV,2,1,06,02,30,050,44,08,,,28,10,48,093,40,18,06,050,43*44XX"
		"$GPGSV,2,2,06,23,30,053,47,27,69,030,38*7AXX"
		"$GPGLL,2769.50673,N,00321.52120,W,140721.00,A,A*78XX"
		"$GPRMC,140722.00,A,2769.50696,N,00321.52135,W,0.219,,290423,,,A*69XX"
		"$GPVTG,,T,,M,0.219,N,0.405,K,A*28XX"
		"$GPGGA,140722.00,2769.50696,N,00321.52135,W,1,05,5.79,130.5,M,48.3,M,,*47XX"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CXX"
		"$GPGSV,2,1,06,02,30,050,44,08,,,28,10,48,093,40,18,06,050,43*44XX"
		"$GPGSV,2,2,06,23,30,053,47,27,69,030,38*7AXX"
		"$GPGLL,2769.50696,N,00321.52135,W,140722.00,A,A*74XX"
		"$GPRMC,140722.00,A,2769.50696,N,0091";

static char Neo6mNonTrackingDataSet[] =
		"VTG,,,,,,,,,N*30XX"
		"$GPGGA,,,,,,0,00,99.99,,,,,"
		",*48XX"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30XX"
		"$GPGSV,1,1,00*79XX"
		"$GPGLL,,,,,,V,N*64XX"
		"$GPRMC,,V,,,,,,,,,,N*53XX"
		"$GPVTG,,,,,,,,,N*30XX"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48XX"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30XX"
		"$GPGSV,1,1,00*79XX"
		"$GPGLL,,,,,,V,N*64XX"
		"$GPRMC,,V,,,,,,,,,,N*53XX"
		"$GPVTG,,,,,,,,,N*30XX"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48XX"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30XX"
		"$GPGSV,1,1,00*79XX"
		"$GPGLL,,,,,,V,N*64XX"
		"$GPRMC,,V,,,,,,,,,,N*53XX"
		"$GPVTG,,,,,,,,,N*30XX"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48XX"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30XX"
		"$GPGSV,1,1,00*79XX"
		"$GPGLL,,,,,,V,N*64XX"
		"$GPRMC,,V,,,,,,,,,,N*53XX"
		"$GPVTG,,,,,,,,,N*30XX"
		"$GPGGA,,,,,,0,00,99.99,,,,,,*48XX"
		"$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30XX"
		"$GPGSV";


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

/*-----------------------------------------------------------------------------------------*/
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



/*
 *
 * ->Null Pointer is reported

 *	->Overflow is detected and reported
 *	->IO Error is reported*/

