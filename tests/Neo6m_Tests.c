#include "Neo6m_MockIO.h"

#include "unity.h"
#include "unity_fixture.h"

#include "Neo6mLiteFlex.h"

#include "Neo6mPrivates.h"

static Neo6mLiteFlex_t Neo6m;

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
TEST_GROUP(Neo6m_IOReadIntoToRingBuffer);

TEST_SETUP(Neo6m_IOReadIntoToRingBuffer)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
	Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);
}

TEST_TEAR_DOWN(Neo6m_IOReadIntoToRingBuffer)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_IOReadIntoToRingBuffer,DataIsWrittenOnEmptyBuffer)
{
	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);

	IOReadIntoToRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	lwrb_t* pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);
	uint8_t* ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);

	TEST_ASSERT_EQUAL(pRingBuf->w,NEO6M_BATCH_SIZE);
	TEST_ASSERT_EQUAL(NEO6M_BATCH_SIZE,lwrb_get_full(pRingBuf));
	TEST_ASSERT_EQUAL_CHAR_ARRAY(Neo6mTrackingDataSet,ByteArray,NEO6M_BATCH_SIZE);
}

TEST(Neo6m_IOReadIntoToRingBuffer,DataIsAppendedOnPartiallyFullBuffer)
{

}
/*
 * ->Data is written on an empty buffer
 *
 * ->Data is appended on a partially full buffer
 *
 * ->Null Pointer is reported
 *->IO Error is reported
 *->Overflow is detected and reported*/

