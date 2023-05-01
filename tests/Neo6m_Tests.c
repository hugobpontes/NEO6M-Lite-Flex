#include "Neo6m_MockIO.h"

#include "unity.h"
#include "unity_fixture.h"

#include "Neo6mLiteFlex.h"

#include "Neo6mPrivates.h"

static Neo6mLiteFlex_t Neo6m;


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
TEST_GROUP(Neo6m_CopyFromBufferToRingBuffer);

TEST_SETUP(Neo6m_CopyFromBufferToRingBuffer)
{
	MockNeo6m_Create(20);
	Neo6m = Neo6mLiteFlex_Create();
}

TEST_TEAR_DOWN(Neo6m_CopyFromBufferToRingBuffer)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_CopyFromBufferToRingBuffer,DataIsWrittenOnEmptyBuffer)
{
	CopyFromBufferToRingBuffer(Neo6m,NEO6M_BATCH_SIZE);
	TEST_ASSERT(0);
}

