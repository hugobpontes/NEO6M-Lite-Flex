#include <stdbool.h>

#include "Neo6m_MockIO.h"

#include "unity.h"
#include "unity_fixture.h"

#include "Neo6mLiteFlex.h"

#include "Neo6mPrivates.h"
#include "TestDataSets.h"

static Neo6mLiteFlex_t Neo6m;
static lwrb_t* pRingBuf;
static uint8_t* ByteArray;
static Neo6mLiteFlexStatus_t Status;
static uint8_t ExpectedBuf[NEO6M_BATCH_BUFFER_SIZE];
static uint8_t ActualBuf[NEO6M_BATCH_BUFFER_SIZE];
static size_t OldReadPointer;
static size_t NewReadPointer;
static Neo6mDefaultMsg_t ActualMsg;
static Neo6mDefaultMsg_t DefaultMsg = NEO6M_MSG_INIT;
static Neo6mDefaultMsg_t ExpectedMsg;
static Neo6mMsgArray_t ReceivedMsgArray= {NEO6M_MSG_INIT,NEO6M_MSG_INIT,NEO6M_MSG_INIT,NEO6M_MSG_INIT};


#define STD_NEO6M_SETUP {\
MockNeo6m_Create(20);\
Neo6m = Neo6mLiteFlex_Create();\
Neo6mLiteFlex_SetIORead(Neo6m, MockNeo6m_Read);\
pRingBuf = Neo6mLiteFlex_GetRingBuffPtr(Neo6m);\
ByteArray = Neo6mLiteFlex_GetByteArray(Neo6m);\
}

#define NON_TRACKING(ExpectedMsg) { \
ExpectedMsg.GPRMC.Status = 'V';\
ExpectedMsg.GPRMC.DataStatus = 'N';\
ExpectedMsg.GPVTG.DataStatus = 'N';\
ExpectedMsg.GPGGA.GpsQuality = 0;\
ExpectedMsg.GPGGA.SatsInView = 0;\
ExpectedMsg.GPGGA.HDOP = 99.99;\
ExpectedMsg.GPGSA.ModeMA = 'A';\
ExpectedMsg.GPGSA.Mode123 = '1';\
ExpectedMsg.GPGSA.PDOP = 99.99;\
ExpectedMsg.GPGSA.VDOP = 99.99;\
ExpectedMsg.GPGSA.HDOP = 99.99;\
ExpectedMsg.GPGSV[0].GPGSVSentences = 1;\
ExpectedMsg.GPGSV[0].SentenceIndex = 1;\
ExpectedMsg.GPGSV[0].SatsInView = 0;\
ExpectedMsg.GPGLL.DataStatus = 'V';\
ExpectedMsg.GPGLL.FAAModeIndicator = 'N';\
}

static bool CompareTime(Neo6mLiteFlex_Time_t* A, Neo6mLiteFlex_Time_t* B)
{
	bool IsEqual = true;
	if (A->Hours != B->Hours){
		IsEqual = false;
	}
	else if (A->Minutes != B->Minutes){
		IsEqual = false;
	}
	else if (A->Seconds != B->Seconds){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareDate(Neo6mLiteFlex_Date_t* A, Neo6mLiteFlex_Date_t* B)
{
	bool IsEqual = true;
	if (A->Day != B->Day){
		IsEqual = false;
	}
	else if (A->Month != B->Month){
		IsEqual = false;
	}
	else if (A->Year != B->Year){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareDegDecMinutes(Neo6mLiteFlex_DegDecMinutes_t* A, Neo6mLiteFlex_DegDecMinutes_t* B)
{
	bool IsEqual = true;
	if (A->DecimalMinutes != B->DecimalMinutes){
		IsEqual = false;
	}
	else if (A->Degrees != B->Degrees){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareSatInfo(Neo6mLiteFlex_GPGSV_SatInfo_t* A, Neo6mLiteFlex_GPGSV_SatInfo_t* B)
{
	bool IsEqual = true;
	if (A->Azimuth != B->Azimuth){
		IsEqual = false;
	}
	else if (A->Elevation != B->Elevation){
		IsEqual = false;
	}
	else if (A->PRN != B->PRN){
		IsEqual = false;
	}
	else if (A->SNR != B->SNR){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareGPRMC(Neo6mLiteFlex_GPRMC_t* A, Neo6mLiteFlex_GPRMC_t* B)
{
	bool IsEqual = true;
	if (A->DataStatus != B->DataStatus){
		IsEqual = false;
	}
	else if (A->EW != B->EW){
		IsEqual = false;
	}
	else if (A->EW_MV != B->EW_MV){
		IsEqual = false;
	}
	else if (A->MagneticVariation != B->MagneticVariation){
		IsEqual = false;
	}
	else if (A->NS!= B->NS){
		IsEqual = false;
	}
	else if (A->Speed!= B->Speed){
		IsEqual = false;
	}
	else if (A->Status != B->Status){
		IsEqual = false;
	}
	else if (A->TrackMadeGood != B->TrackMadeGood){
		IsEqual = false;
	}
	else if (!CompareDate(&A->Date,&B->Date)){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Latitude,&B->Latitude)){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Longitude,&B->Longitude)){
		IsEqual = false;
	}
	else if (!CompareTime(&A->UtcTime,&B->UtcTime)){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareGPVTG(Neo6mLiteFlex_GPVTG_t* A, Neo6mLiteFlex_GPVTG_t* B)
{
	bool IsEqual = true;
	if (A->DataStatus != B->DataStatus){
		IsEqual = false;
	}
	else if (A->MagneticTrackDegrees != B->MagneticTrackDegrees){
		IsEqual = false;
	}
	else if (A->SpeedKnots != B->SpeedKnots){
		IsEqual = false;
	}
	else if (A->SpeedKph != B->SpeedKph){
		IsEqual = false;
	}
	else if (A->TrueTrackDegrees != B->TrueTrackDegrees){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareGPGGA(Neo6mLiteFlex_GPGGA_t* A, Neo6mLiteFlex_GPGGA_t* B)
{
	bool IsEqual = true;
	if (A->AntennaAltitude!= B->AntennaAltitude){
		IsEqual = false;
	}
	else if (A->EW!= B->EW){
		IsEqual = false;
	}
	else if (A->GeoIdalSeparation!= B->GeoIdalSeparation){
		IsEqual = false;
	}
	else if (A->GpsDataAge!= B->GpsDataAge){
		IsEqual = false;
	}
	else if (A->GpsQuality!= B->GpsQuality){
		IsEqual = false;
	}
	else if (A->HDOP!= B->HDOP){
		IsEqual = false;
	}
	else if (A->NS!= B->NS){
		IsEqual = false;
	}
	else if (A->RefStationId!= B->RefStationId){
		IsEqual = false;
	}
	else if (A->SatsInView!= B->SatsInView){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Latitude,&B->Latitude)){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Longitude,&B->Longitude)){
		IsEqual = false;
	}
	else if (!CompareTime(&A->UtcTime,&B->UtcTime)){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareGPGSA(Neo6mLiteFlex_GPGSA_t* A, Neo6mLiteFlex_GPGSA_t* B)
{
	bool IsEqual = true;
	if (A->HDOP != B->HDOP){
		IsEqual = false;
	}
	else if (A->Mode123 != B->Mode123){
		IsEqual = false;
	}
	else if (A->ModeMA != B->ModeMA){
		IsEqual = false;
	}
	else if (A->PDOP != B->PDOP){
		IsEqual = false;
	}
	else if (A->VDOP != B->VDOP){
		IsEqual = false;
	}
	else if (memcmp(A->PRN,A->PRN,12*sizeof(uint16_t))){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareGPGSV(Neo6mLiteFlex_GPGSV_t* A, Neo6mLiteFlex_GPGSV_t* B)
{
	bool IsEqual = true;
	if (A->GPGSVSentences != B->GPGSVSentences){
		IsEqual = false;
	}
	else if (A->SatsInView != B->SatsInView){
		IsEqual = false;
	}
	else if (A->SentenceIndex != B->SentenceIndex){
		IsEqual = false;
	}
	else if (!CompareSatInfo(&A->SatInfo[0],&B->SatInfo[0])){
		IsEqual = false;
	}
	else if (!CompareSatInfo(&A->SatInfo[1],&B->SatInfo[1])){
		IsEqual = false;
	}
	else if (!CompareSatInfo(&A->SatInfo[2],&B->SatInfo[2])){
		IsEqual = false;
	}
	else if (!CompareSatInfo(&A->SatInfo[3],&B->SatInfo[3])){
		IsEqual = false;
	}
	return IsEqual;

}
static bool CompareGPGLL(Neo6mLiteFlex_GPGLL_t* A, Neo6mLiteFlex_GPGLL_t* B)
{
	bool IsEqual = true;
	if (A->DataStatus != B->DataStatus){
		IsEqual = false;
	}
	else if (A->EW != B->EW){
		IsEqual = false;
	}
	else if (A->FAAModeIndicator != B->FAAModeIndicator){
		IsEqual = false;
	}
	else if (A->NS != B->NS){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Latitude,&B->Latitude)){
		IsEqual = false;
	}
	else if (!CompareDegDecMinutes(&A->Longitude,&B->Longitude)){
		IsEqual = false;
	}
	else if (!CompareTime(&A->UtcTime,&B->UtcTime)){
		IsEqual = false;
	}
	return IsEqual;
}
static bool CompareDefaultMsg(Neo6mDefaultMsg_t* A, Neo6mDefaultMsg_t* B)
{
	bool IsEqual = true;
	if (!CompareGPRMC(&A->GPRMC,&B->GPRMC)){
		IsEqual = false;
	}
	else if (!CompareGPVTG(&A->GPVTG,&B->GPVTG)){
		IsEqual = false;
	}
	else if (!CompareGPGGA(&A->GPGGA,&B->GPGGA)){
		IsEqual = false;
	}
	else if (!CompareGPGSA(&A->GPGSA,&B->GPGSA)){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[0],&B->GPGSV[0])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[1],&B->GPGSV[1])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[2],&B->GPGSV[2])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[3],&B->GPGSV[3])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[4],&B->GPGSV[4])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[5],&B->GPGSV[5])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[6],&B->GPGSV[6])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[7],&B->GPGSV[7])){
		IsEqual = false;
	}
	else if (!CompareGPGSV(&A->GPGSV[8],&B->GPGSV[8])){
		IsEqual = false;
	}
	else if (!CompareGPGLL(&A->GPGLL,&B->GPGLL)){
		IsEqual = false;
	}
	return IsEqual;
}

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
	STD_NEO6M_SETUP
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
	STD_NEO6M_SETUP

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
	TEST_ASSERT_EQUAL_UINT32(9,GetBytesUntilSequenceEnd(pRingBuf,",0032"));
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
	STD_NEO6M_SETUP

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
	TEST_ASSERT_EQUAL_CHAR('X',GetCharBeforeSequence(pRingBuf,",N,0.364,K,A*2CZZ"));
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
	STD_NEO6M_SETUP

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
	lwrb_write(pRingBuf,TooLargeDataSet,sizeof(TooLargeDataSet));

	SkipAndUpdateTestReadPtr(12);
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(21);
}

TEST(Neo6m_GetFloatUntilSequence,ReturnsErrorIfFloatStringContainsNonNumeric)
{
	/*Empty buffer and fill in specific data, for this test only */
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,NonNumericDataSet,sizeof(NonNumericDataSet));

	SkipAndUpdateTestReadPtr(12);
	TEST_ASSERT_EQUAL_FLOAT(FLOAT_NOT_FOUND,GetFloatUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(13);
}


/*-----------------------------------------------------------------------------------------------------------------*/
TEST_GROUP(Neo6m_GetIntUntilSequence);

TEST_SETUP(Neo6m_GetIntUntilSequence)
{
	STD_NEO6M_SETUP

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_GetIntUntilSequence)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_GetIntUntilSequence,ReturnsIntUntilSequence)
{
	SkipAndUpdateTestReadPtr(120);
	TEST_ASSERT_EQUAL_UINT16(18,GetIntUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(3);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfSequenceIsNext)
{
	SkipAndUpdateTestReadPtr(119);
	TEST_ASSERT_EQUAL_INT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(1);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfSequenceNotFound)
{
	TEST_ASSERT_EQUAL_INT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,"Hello"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfSequenceNull)
{
	TEST_ASSERT_EQUAL_INT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,""));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfSequenceTooLong)
{
	TEST_ASSERT_EQUAL_INT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,",10,48,093,40,18,06"));
	TestReadPointerAdvanced(0);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfBytesUntilStringTooLong)
{
	/*Empty buffer and fill in specific data, for this test only */
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,TooLargeDataSet,sizeof(TooLargeDataSet));

	SkipAndUpdateTestReadPtr(33);
	TEST_ASSERT_EQUAL_FLOAT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(15);
}

TEST(Neo6m_GetIntUntilSequence,ReturnsErrorIfIntStringContainsNonNumeric)
{
	/*Empty buffer and fill in specific data, for this test only */
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,NonNumericDataSet,sizeof(NonNumericDataSet));

	SkipAndUpdateTestReadPtr(25);
	TEST_ASSERT_EQUAL_FLOAT(UINT16_NOT_FOUND,GetIntUntilSequence(pRingBuf,","));
	TestReadPointerAdvanced(5);
}

/*----------------------------------------------------------------------------*/

TEST_GROUP(Neo6m_GetNextBytesAsInt);

TEST_SETUP(Neo6m_GetNextBytesAsInt)
{
	STD_NEO6M_SETUP

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_GetNextBytesAsInt)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_GetNextBytesAsInt,ReturnsNextBytesAsInt)
{
	SkipAndUpdateTestReadPtr(120);
	TEST_ASSERT_EQUAL_UINT16(18,GetNextBytesAsInt(pRingBuf,2));
	TestReadPointerAdvanced(2);
}

TEST(Neo6m_GetNextBytesAsInt,ReturnsErrorIfNonNumeric)
{
	lwrb_skip(pRingBuf,750);
	lwrb_write(pRingBuf,NonNumericDataSet,sizeof(NonNumericDataSet));

	SkipAndUpdateTestReadPtr(25);
	TEST_ASSERT_EQUAL_UINT16(UINT16_NOT_FOUND,GetNextBytesAsInt(pRingBuf,4));
	TestReadPointerAdvanced(4);
}

TEST(Neo6m_GetNextBytesAsInt,ReturnsErrorIfBufferEmpty)
{
	SkipAndUpdateTestReadPtr(750-5);
	TEST_ASSERT_EQUAL_UINT16(UINT16_NOT_FOUND,GetNextBytesAsInt(pRingBuf,6));
	TestReadPointerAdvanced(0);
}

/*----------------------------------------*/
TEST_GROUP(Neo6m_FillInTrackingNeo6mMsgStruct);

TEST_SETUP(Neo6m_FillInTrackingNeo6mMsgStruct)
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

TEST_TEAR_DOWN(Neo6m_FillInTrackingNeo6mMsgStruct)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_FillInTrackingNeo6mMsgStruct,FillsAsExpectedFromTrackingData)
{
	ActualMsg = GetDefaultMsg(pRingBuf);

	ExpectedMsg = ExpectDefaultMsg_TrackingData;

	TEST_ASSERT(CompareDefaultMsg(&ActualMsg,&ExpectedMsg));
}
	/*Test only partially filled struct since buffer empty */
TEST(Neo6m_FillInTrackingNeo6mMsgStruct,FillsAsExpectedFrom2ndTrackingData)
{
	ActualMsg = GetDefaultMsg(pRingBuf);
	ActualMsg = GetDefaultMsg(pRingBuf);

	ExpectedMsg = DefaultMsg;

	TEST_ASSERT(CompareDefaultMsg(&ActualMsg,&ExpectedMsg));
}

/*----------------------------------------------------------------------------------*/

TEST_GROUP(Neo6m_FillInNonTrackingNeo6mMsgStruct);

TEST_SETUP(Neo6m_FillInNonTrackingNeo6mMsgStruct)
{
	STD_NEO6M_SETUP

	ExpectedMsg = DefaultMsg;

	NON_TRACKING(ExpectedMsg)

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mNonTrackingDataSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_FillInNonTrackingNeo6mMsgStruct)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_FillInNonTrackingNeo6mMsgStruct,FillsAsExpectedFromNonTrackingData)
{
	ActualMsg = GetDefaultMsg(pRingBuf);

	TEST_ASSERT(CompareDefaultMsg(&ActualMsg,&ExpectedMsg));
}

/*----------------------------------------*/
TEST_GROUP(Neo6m_FillInTracking1SVNeo6mMsgStruct);

TEST_SETUP(Neo6m_FillInTracking1SVNeo6mMsgStruct)
{
	STD_NEO6M_SETUP

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingData1SVSet, NEO6M_BATCH_SIZE);
	IOReadIntoRingBuffer(Neo6m,NEO6M_BATCH_SIZE);

	ExpectedMsg = ExpectDefaultMsg_Tracking1SVData;

	OldReadPointer = pRingBuf->r;
}

TEST_TEAR_DOWN(Neo6m_FillInTracking1SVNeo6mMsgStruct)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_FillInTracking1SVNeo6mMsgStruct,FillsAsExpectedFromTracking1SVData)
{
	ActualMsg = GetDefaultMsg(pRingBuf);

	TEST_ASSERT(CompareDefaultMsg(&ActualMsg,&ExpectedMsg));
}

/*----------------------------------------*/
TEST_GROUP(Neo6m_MsgArrays);

TEST_SETUP(Neo6m_MsgArrays)
{
	STD_NEO6M_SETUP
}

TEST_TEAR_DOWN(Neo6m_MsgArrays)
{
	MockNeo6m_VerifyComplete();
	MockNeo6m_Destroy();
}

TEST(Neo6m_MsgArrays,TrackingData)
{
	uint32_t MsgsRead;

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mTrackingDataSet, NEO6M_BATCH_SIZE);

	ExpectedMsg = ExpectDefaultMsg_TrackingData;

	MsgsRead = Neo6mLiteFlex_GetNeo6mMsgs(Neo6m,&ReceivedMsgArray);

	TEST_ASSERT_EQUAL_UINT32(1,MsgsRead);
	TEST_ASSERT(CompareDefaultMsg(&ReceivedMsgArray[0],&ExpectedMsg));

}

TEST(Neo6m_MsgArrays,NonTrackingData)
{
	uint32_t MsgsRead;

	MockNeo6m_ExpectReadAndReturn(NEO6M_BATCH_SIZE, Neo6mNonTrackingDataSet, NEO6M_BATCH_SIZE);

	ExpectedMsg = DefaultMsg;

	NON_TRACKING(ExpectedMsg)

	MsgsRead = Neo6mLiteFlex_GetNeo6mMsgs(Neo6m,&ReceivedMsgArray);

	TEST_ASSERT_EQUAL_UINT32(3,MsgsRead);
	TEST_ASSERT(CompareDefaultMsg(&ReceivedMsgArray[0],&ExpectedMsg));
	TEST_ASSERT(CompareDefaultMsg(&ReceivedMsgArray[1],&ExpectedMsg));
	TEST_ASSERT(CompareDefaultMsg(&ReceivedMsgArray[2],&ExpectedMsg));

}

/*Test for non tracking */
