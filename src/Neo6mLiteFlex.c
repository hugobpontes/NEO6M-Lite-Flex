/*
 * Neo6m.c
 *
 *  Created on: May 1, 2023
 *      Author: Utilizador
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "Neo6mLiteFlex.h"
/**
 * @brief Macro defining the maximum size of a sequence that can be sought
 */
#define MAX_SEQUENCE_SIZE 10
/**
 * @brief Macro defining the maximum number of chars that a string representing a float
 * can have, including the decimal point
 */
#define MAX_FLOAT_STRING_SIZE 15
#define MAX_INT_STRING_SIZE 15
/**
 * @brief Macro that returns the minimum of two values
 */
#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

 #define max(a,b) 				 \
({								 \
		__typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
		_a > _b ? _a : _b; 		 \
})

/**
 * @brief Struct that will only be visible to the outside as an abstract type. Represents an instance of an
 * Neo6mLiteFlex object.
 */
typedef struct Neo6mLiteFlexStruct
{
	lwrb_t MessageRingBuffer; /**< neo6m ring buffer (use to store received messages) handle */
	uint8_t MessageByteArray[NEO6M_BATCH_BUFFER_SIZE]; /**<Section of memory where neo6m msgs are stored when using the neo6m msg lwrb */
	IOFunc_t pIORead; /**< Read function pointer that points to a function used to read neo 6m uart msgs */
} Neo6mLiteFlexStruct_t;
/**
 * @brief Sets the function pointer used to read neo 6m uart msgs
 *
 * @param Neo6mLiteFlex Neo6mLiteFlex object/instance being set
 * @param pIORead Function pointer pointing to the function that is used to read uart msgs from neo 6m
 */
void Neo6mLiteFlex_SetIORead(Neo6mLiteFlex_t Neo6mLiteFlex, IOFunc_t pIORead)
{
	Neo6mLiteFlex->pIORead=pIORead;
}
/**
 * @brief Returns a pointer to the ring buffer handle of a Neo6mLiteFlex instance
 *
 * @param Neo6mLiteFlex Neo6mLiteFlex object/instance being accessed
 *
 * @return Pointer to the neo 6m msg ring buffer handle
 */
lwrb_t* Neo6mLiteFlex_GetRingBuffPtr(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	return &(Neo6mLiteFlex->MessageRingBuffer);
}
/**
 * @brief Returns a pointer to beginn of a byte array of a Neo6mLiteFlex instance
 *
 * @param Neo6mLiteFlex Neo6mLiteFlex object/instance being accessed
 *
 * @return Pointer to the beginning of neo 6m msg byte array
 */
uint8_t* Neo6mLiteFlex_GetByteArray(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	return Neo6mLiteFlex->MessageByteArray;
}
/**
 * @brief Create neo6m lite flex instance by allocating memory for Neo6mLiteFlexStruct_t struct
 * This functions allows user of this library to use Neo6mLiteFlexStruct_t in an abstracted way
 *
 * @return pointer to memory allocated to Neo6mLiteFlexStruct_t, acts as a mpu6050flex handle
 */
Neo6mLiteFlex_t Neo6mLiteFlex_Create()
{
	Neo6mLiteFlex_t Neo6mLiteFlex;
	Neo6mLiteFlex = (Neo6mLiteFlex_t) malloc(sizeof(Neo6mLiteFlexStruct_t));

	lwrb_init(&(Neo6mLiteFlex->MessageRingBuffer),Neo6mLiteFlex->MessageByteArray,NEO6M_BATCH_BUFFER_SIZE);
	Neo6mLiteFlex->pIORead = NULL;

	return Neo6mLiteFlex;
}
/**
 * @brief Destroy neo6m instance by freeing memory held by Neo6mLiteFlexStruct_t
 *
 * @param Neo6mLiteFlex Neo6mLiteFlex object/instance being destroyed
 */
void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex)
{
	free(Neo6mLiteFlex);
}
/**
 * @brief Read UART data directly into the neo 6m message ring buffer
 *
 * @param Neo6mLiteFlex Neo6mLiteFlex object/instance used accessed
 * @param CopySize amount of bytes to be read and copied to ring buffer
 *
 * @return Success code indicating success or lack thereof of the operation
 *
 */
UT_STATIC Neo6mLiteFlexStatus_t IOReadIntoRingBuffer(Neo6mLiteFlex_t Neo6mLiteFlex, size_t CopySize)
{
	Neo6mLiteFlexStatus_t Status = NEO6M_SUCCESS;

	lwrb_t* pRingBuf;
	uint8_t* ByteArray;
	size_t ReadBytes = 0;
	size_t FirstReadSize;
	size_t SecondReadSize;
	size_t FreeBytes;

	pRingBuf = &(Neo6mLiteFlex->MessageRingBuffer);
	ByteArray = Neo6mLiteFlex->MessageByteArray;
	FreeBytes = lwrb_get_free(pRingBuf);

	if (Neo6mLiteFlex->pIORead)
	{
		if(CopySize < FreeBytes)
		{
			/*Since buffer is circular and we are doing linear reads, we may need to do two separate reads*/
			FirstReadSize = min(lwrb_get_linear_block_write_length(pRingBuf),CopySize);
			SecondReadSize = CopySize-FirstReadSize;

			ReadBytes += Neo6mLiteFlex->pIORead(ByteArray+pRingBuf->w,FirstReadSize);
			lwrb_advance(pRingBuf, FirstReadSize);
			if (SecondReadSize)
			{
				ReadBytes += Neo6mLiteFlex->pIORead(ByteArray+pRingBuf->w,SecondReadSize);
				lwrb_advance(pRingBuf, SecondReadSize);
			}

			if (ReadBytes != CopySize)
			{
				Status = NEO6M_IO_ERROR;
			}
		}
		else
		{
			Status = NEO6M_BUFFER_OVERFLOW;
		}

	}
	else
	{
		Status = NEO6M_NULL_PTR;
	}

	return Status;
}
/**
 * @brief Peeks into a ring buffer to obtain a string and compares it to input string
 *
 * @param pRingBuf Pointer to the ring buffer where chars to be compared are stored
 * @param Sequence Sequence to with which chars in the ring buffer are compared
 * @param SequenceLength Length in bytes of the sequence parameter
 * @param Offset How many far into the ring buffer should the comparison be performed
 *
 * @return Boolean value indicating if there is a match between the strings
 *
 */
static bool PeekIntoStringAndCompare (lwrb_t* pRingBuf, char* Sequence, uint32_t SequenceLength, size_t Offset)
{
		char SequenceBuffer[MAX_SEQUENCE_SIZE];

		lwrb_peek(pRingBuf,Offset,SequenceBuffer,SequenceLength);

		return !(strncmp(SequenceBuffer,Sequence,SequenceLength));
}
/**
 * @brief Returns number of bytes in a ring buffer between current position and provided sequence.
 * Returns SEQUENCE_NOT_FOUND if sequence was not found
 *
 * @param pRingBuf Pointer to the ring buffer where chars to be compared are stored
 * @param Sequence Sequence to with which chars in the ring buffer are compared
 * @param SequenceLength Length in bytes of the sequence parameter
 *
 * @return Number of bytes in a ring buffer between current position and provided sequence
 *
 */
static uint32_t GetBytesUntilSequence(lwrb_t* pRingBuf, char* Sequence, uint32_t SequenceLength)
{
	uint32_t BytesUntilSeq = SEQUENCE_NOT_FOUND;
	uint32_t PositionChecked = 0;

	uint32_t FullBytes = 0;

	if (Sequence && SequenceLength && (SequenceLength<MAX_SEQUENCE_SIZE) && pRingBuf )
	{
		FullBytes = lwrb_get_full(pRingBuf);
		if (SequenceLength < FullBytes)
		{
			/*Peek bytes equal to the size of the fed sequence until a match is found*/
			while (PositionChecked<(FullBytes-SequenceLength))
			{
				if(PeekIntoStringAndCompare(pRingBuf,Sequence,SequenceLength,PositionChecked))
				{
					BytesUntilSeq = PositionChecked;
					break;
				}
				PositionChecked++;

			}

		}
	}

	return BytesUntilSeq;
}
/**
 * @brief Returns number of bytes in a ring buffer between current position and end of provided sequence
 * Returns SEQUENCE_NOT_FOUND if sequence was not found
 *
 * @param pRingBuf Pointer to the ring buffer where chars are stored
 * @param Sequence Sequence to with which chars in the ring buffer are compared
 * @param SequenceLength Length in bytes of the sequence parameter
 *
 * @return Number of bytes in a ring buffer between current position and end of provided sequence
 *
 */
UT_STATIC uint32_t GetBytesUntilSequenceEnd(lwrb_t* pRingBuf, char* Sequence)
{
	uint32_t SequenceLength = strlen(Sequence);

	uint32_t BytesUntilSeqEnd = GetBytesUntilSequence(pRingBuf,Sequence,SequenceLength);

	if (BytesUntilSeqEnd != SEQUENCE_NOT_FOUND)
	{
		BytesUntilSeqEnd += SequenceLength; /*Simply add sequence length to get bytes until end*/
	}

	return BytesUntilSeqEnd;
}
/**
 * @brief Reads char after a provided number of bytes in the ring buffer
 *
 * @param pRingBuf Pointer to the ring buffer with chars
 * @param SkippedBytes Bytes to be skipped before reading char
 * @param pChar pointer to which char is read
 *
 */
static inline void ReadCharAfter(lwrb_t* pRingBuf, size_t SkippedBytes, char* pChar)
{
	lwrb_skip(pRingBuf,SkippedBytes-1);
	lwrb_read(pRingBuf,pChar,1);
}
/**
 * @brief Returns char in a given ring buffer that is immediately before a provided sequence
 * Returns CHAR_NOT_FOUND if unable to find chars between current ring buffer position and provided sequence
 * or if sequence is invalid or inexistent
 *
 * @param pRingBuf Pointer to the ring buffer where chars are stored
 * @param Sequence Sequence before which the returned char is
 *
 * @return Char sitting immediately before the provided sequence
 *
 */
UT_STATIC char GetCharBeforeSequence(lwrb_t* pRingBuf,char* Sequence)
{
	char CharBeforeSeq = CHAR_NOT_FOUND;
	uint32_t SequenceLength = strlen(Sequence);

	if (pRingBuf && Sequence)
	{
		uint32_t BytesUntilSeq = GetBytesUntilSequence(pRingBuf,Sequence,SequenceLength);

		if (BytesUntilSeq != SEQUENCE_NOT_FOUND)
		{
			if (BytesUntilSeq != 0)
			{
				ReadCharAfter(pRingBuf,BytesUntilSeq,&CharBeforeSeq);
			}
			/*Skip sequence length even if no data is found */
			lwrb_skip(pRingBuf,SequenceLength);
		}
	}

	return CharBeforeSeq;
}
/**
 * @brief Converts bytes in a ring buffer into a float and returns data as float
 * Returns FLOAT_NOT_FOUND if unable to find valid floating point string inside provided
 * byte number after current ring buffer position
 *
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 * @param CharsNbr amount of bytes to be read and converted to float
 *
 * @return float number equivalent of provided char array
 *
 */
static float ConvertCharsToFloat(lwrb_t* pRingBuf, uint32_t CharsNbr)
{
	float ConvertedFloat = FLOAT_NOT_FOUND;
	char FloatString[MAX_FLOAT_STRING_SIZE];
	char* pLastValidChar;

	lwrb_read(pRingBuf, FloatString, CharsNbr);
	FloatString[CharsNbr] = '\0'; // Make Float char array a String
	ConvertedFloat = strtof(FloatString, &pLastValidChar);
	if ((pLastValidChar-FloatString) != CharsNbr)
	{
		ConvertedFloat = FLOAT_NOT_FOUND; //Signal invalid float
	}

	return ConvertedFloat;
}
/**
 * @brief Converts chars between current position and provided sequence into a floating point number
 * Returns FLOAT_NOT_FOUND if sequence is invalid or inexistent or if chars inbetween do not correspond to a
 * valid floating point number.
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 * @param Sequence Sequence until which chars are converted to float
 *
 * @return float number equivalent of chars between current ring buffer position and sequence
 *
 */
UT_STATIC float GetFloatUntilSequence(lwrb_t* pRingBuf,char* Sequence)
{
	float FloatUntilSequence = FLOAT_NOT_FOUND;

	uint32_t SequenceLength = strlen(Sequence);
	char* pLastValidChar;

	uint32_t BytesUntilSeq = GetBytesUntilSequence(pRingBuf,Sequence,SequenceLength);

	if (BytesUntilSeq != SEQUENCE_NOT_FOUND)
	{
		if (BytesUntilSeq && BytesUntilSeq<MAX_FLOAT_STRING_SIZE)
		{
			FloatUntilSequence = ConvertCharsToFloat(pRingBuf,BytesUntilSeq);
		}
		else
		{
			lwrb_skip(pRingBuf,BytesUntilSeq); /*Skip chars found until sequence if invalid float string size*/
		}
		lwrb_skip(pRingBuf,SequenceLength); /*Skip sequence in both cases */
	}

	return FloatUntilSequence;
}

static uint16_t ConvertCharsToInt(lwrb_t* pRingBuf, uint32_t CharsNbr)
{
	uint16_t ConvertedInt = UINT16_NOT_FOUND;
	char IntString[MAX_INT_STRING_SIZE];
	char* pLastValidChar;

	lwrb_read(pRingBuf, IntString, CharsNbr);
	IntString[CharsNbr] = '\0'; // Make Float char array a String
	ConvertedInt = strtoul(IntString, &pLastValidChar,10);
	if ((pLastValidChar-IntString) != CharsNbr)
	{
		ConvertedInt = UINT16_NOT_FOUND; //Signal invalid float
	}

	return ConvertedInt;
}

UT_STATIC uint16_t GetIntUntilSequence(lwrb_t* pRingBuf,char* Sequence)
{
	uint16_t IntUntilSequence = UINT16_NOT_FOUND;

	uint32_t SequenceLength = strlen(Sequence);
	char* pLastValidChar;

	uint32_t BytesUntilSeq = GetBytesUntilSequence(pRingBuf,Sequence,SequenceLength);

	if (BytesUntilSeq != SEQUENCE_NOT_FOUND)
	{
		if (BytesUntilSeq && BytesUntilSeq<MAX_INT_STRING_SIZE)
		{
			IntUntilSequence = ConvertCharsToInt(pRingBuf,BytesUntilSeq);
		}
		else
		{
			lwrb_skip(pRingBuf,BytesUntilSeq); /*Skip chars found until sequence if invalid float string size*/
		}
		lwrb_skip(pRingBuf,SequenceLength); /*Skip sequence in both cases */
	}

	return IntUntilSequence;
}

UT_STATIC uint16_t GetNextBytesAsInt(lwrb_t* pRingBuf,uint32_t BytesToRead)
{
	uint16_t NextBytesAsInt = UINT16_NOT_FOUND;
	uint32_t FullBytes;

	char IntString[MAX_INT_STRING_SIZE];

	if (pRingBuf && BytesToRead && (BytesToRead < MAX_INT_STRING_SIZE))
	{
		FullBytes = lwrb_get_full(pRingBuf);
		if (FullBytes >= BytesToRead)
		{
			NextBytesAsInt = ConvertCharsToInt(pRingBuf,BytesToRead);
		}
	}

	return NextBytesAsInt;
}

static void SkipUntilEndOfSequence(lwrb_t* pRingBuf, char* Sequence)
{
	uint32_t BytesToSkip;
	BytesToSkip = GetBytesUntilSequenceEnd(pRingBuf,Sequence);
	lwrb_skip(pRingBuf,BytesToSkip);
}

static Neo6mLiteFlex_Time_t GetTime(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_Time_t ReturnTime;

	ReturnTime.Hours = GetNextBytesAsInt(pRingBuf,2);
	ReturnTime.Minutes = GetNextBytesAsInt(pRingBuf,2);
	ReturnTime.Seconds = GetFloatUntilSequence(pRingBuf,",");

	return ReturnTime;
}

static Neo6mLiteFlex_Date_t GetDate(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_Date_t ReturnDate;

	ReturnDate.Day = GetNextBytesAsInt(pRingBuf,2);
	ReturnDate.Month = GetNextBytesAsInt(pRingBuf,2);
	ReturnDate.Year = GetNextBytesAsInt(pRingBuf,2);
	if (ReturnDate.Year != UINT16_NOT_FOUND)
	{
		ReturnDate.Year+= 2000; /*Sadly will only work until 2099 :( */
	}
	return ReturnDate;
}

static Neo6mLiteFlex_DegDecMinutes_t GetDegDecMinutes(lwrb_t* pRingBuf, uint8_t DegDigits)
{
	Neo6mLiteFlex_DegDecMinutes_t ReturnDegDecMinutes;

	ReturnDegDecMinutes.Degrees = GetNextBytesAsInt(pRingBuf,DegDigits);
	ReturnDegDecMinutes.DecimalMinutes = GetFloatUntilSequence(pRingBuf,",");

	return ReturnDegDecMinutes;
}

static Neo6mLiteFlex_GPGSV_SatInfo_t GetSatInfo(lwrb_t* pRingBuf, bool IsLastFlag)
{
	Neo6mLiteFlex_GPGSV_SatInfo_t ReturnSatInfo;

	ReturnSatInfo.PRN = GetIntUntilSequence(pRingBuf,",");
	ReturnSatInfo.Elevation = GetIntUntilSequence(pRingBuf,",");
	ReturnSatInfo.Azimuth = GetIntUntilSequence(pRingBuf,",");
	if (IsLastFlag)
	{
		ReturnSatInfo.SNR = GetIntUntilSequence(pRingBuf,"*");
	}
	else
	{
		ReturnSatInfo.SNR = GetIntUntilSequence(pRingBuf,",");
	}

	return ReturnSatInfo;
}

static Neo6mLiteFlex_GPRMC_t GetGPRMC(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPRMC_t ReturnGPRMC = GPRMC_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPRMC,");

	ReturnGPRMC.UtcTime = GetTime(pRingBuf);
	ReturnGPRMC.Status = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPRMC.Latitude = GetDegDecMinutes(pRingBuf,2);
	ReturnGPRMC.NS = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPRMC.Longitude = GetDegDecMinutes(pRingBuf,3);
	ReturnGPRMC.EW = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPRMC.Speed = GetFloatUntilSequence(pRingBuf,",");
    ReturnGPRMC.TrackMadeGood = GetFloatUntilSequence(pRingBuf,",");
    ReturnGPRMC.Date =  GetDate(pRingBuf);
    ReturnGPRMC.MagneticVariation = GetFloatUntilSequence(pRingBuf, ",");
    ReturnGPRMC.EW_MV = GetCharBeforeSequence(pRingBuf,",");
    ReturnGPRMC.DataStatus = GetCharBeforeSequence(pRingBuf,"*");

    return ReturnGPRMC;
}

static Neo6mLiteFlex_GPVTG_t GetGPVTG(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPVTG_t ReturnGPVTG = GPVTG_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPVTG,");

	ReturnGPVTG.TrueTrackDegrees = GetFloatUntilSequence(pRingBuf,",");
	lwrb_skip(pRingBuf,2);
	ReturnGPVTG.MagneticTrackDegrees = GetFloatUntilSequence(pRingBuf,",");
	lwrb_skip(pRingBuf,2);
	ReturnGPVTG.SpeedKnots = GetFloatUntilSequence(pRingBuf,",");
	lwrb_skip(pRingBuf,2);
	ReturnGPVTG.SpeedKph = GetFloatUntilSequence(pRingBuf,",");
	lwrb_skip(pRingBuf,2);
	ReturnGPVTG.DataStatus = GetCharBeforeSequence(pRingBuf,"*");

	return ReturnGPVTG;

}

static Neo6mLiteFlex_GPGGA_t GetGPGGA(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPGGA_t ReturnGPGGA = GPGGA_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPGGA,");

	ReturnGPGGA.UtcTime = GetTime(pRingBuf);
	ReturnGPGGA.Latitude = GetDegDecMinutes(pRingBuf,2);
	ReturnGPGGA.NS = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGGA.Longitude = GetDegDecMinutes(pRingBuf,3);
	ReturnGPGGA.EW = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGGA.GpsQuality = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.SatsInView = GetIntUntilSequence(pRingBuf, ",");
	ReturnGPGGA.HDOP = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.AntennaAltitude = GetFloatUntilSequence(pRingBuf, ",");
	lwrb_skip(pRingBuf,2);
	ReturnGPGGA.GeoIdalSeparation = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.GpsDataAge = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.RefStationId = GetIntUntilSequence(pRingBuf, "*");

	return ReturnGPGGA;
}

static Neo6mLiteFlex_GPGSA_t GetGPGSA(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPGSA_t ReturnGPGSA = GPGSA_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPGSA,");

	ReturnGPGSA.ModeMA = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGSA.Mode123 = GetCharBeforeSequence(pRingBuf,",");
	for (int idx = 0; idx<12;idx++)
	{
		ReturnGPGSA.PRN[idx] = GetIntUntilSequence(pRingBuf,",");
	}

	ReturnGPGSA.PDOP = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGSA.HDOP = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGSA.VDOP = GetFloatUntilSequence(pRingBuf, "*");

	return ReturnGPGSA;
}

static Neo6mLiteFlex_GPGSV_t GetGPGSV(lwrb_t* pRingBuf, uint32_t* SatsParsed)
{
	Neo6mLiteFlex_GPGSV_t ReturnGPGSV = GPGSV_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPGSV,");

	ReturnGPGSV.GPGSVSentences = GetIntUntilSequence(pRingBuf, ",");
	ReturnGPGSV.SentenceIndex = GetIntUntilSequence(pRingBuf, ",");
	ReturnGPGSV.SatsInView = GetIntUntilSequence(pRingBuf, ",");

	uint32_t SatsToParse = min(4,ReturnGPGSV.SatsInView-*SatsParsed);

	for (int idx = 0;idx<SatsToParse;idx++)
	{
		if (idx != (SatsToParse-1)) //Check if its last sat info in a sentence
		{
			ReturnGPGSV.SatInfo[idx] = GetSatInfo(pRingBuf, false);
		}
		else
		{
			ReturnGPGSV.SatInfo[idx] = GetSatInfo(pRingBuf, true);
		}
	}

	*SatsParsed+=SatsToParse;

	return ReturnGPGSV;
}

static Neo6mLiteFlex_GPGLL_t GetGPGLL(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPGLL_t ReturnGPGLL = GPGLL_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPGLL,");

	ReturnGPGLL.Latitude = GetDegDecMinutes(pRingBuf,2);
	ReturnGPGLL.NS = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGLL.Longitude = GetDegDecMinutes(pRingBuf,3);
	ReturnGPGLL.EW = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGLL.UtcTime = GetTime(pRingBuf);
	ReturnGPGLL.DataStatus = GetCharBeforeSequence(pRingBuf,",");
	ReturnGPGLL.FAAModeIndicator = GetCharBeforeSequence(pRingBuf,"*");

	return ReturnGPGLL;
}

UT_STATIC Neo6mDefaultMsg_t GetDefaultMsg(lwrb_t* pRingBuf)
{
	Neo6mDefaultMsg_t ReturnMsg = NEO6M_MSG_INIT;

	uint32_t GPGSV_SatsParsed = 0;

	ReturnMsg.GPRMC 	= GetGPRMC(pRingBuf);
	ReturnMsg.GPVTG 	= GetGPVTG(pRingBuf);
	ReturnMsg.GPGGA 	= GetGPGGA(pRingBuf);
	ReturnMsg.GPGSA 	= GetGPGSA(pRingBuf);
	ReturnMsg.GPGSV[0] 	= GetGPGSV(pRingBuf,&GPGSV_SatsParsed);
	if (ReturnMsg.GPGSV[0].GPGSVSentences != UINT16_NOT_FOUND){
		for (int idx = 1; idx<ReturnMsg.GPGSV[0].GPGSVSentences;idx++)
		{
			ReturnMsg.GPGSV[idx] = GetGPGSV(pRingBuf,&GPGSV_SatsParsed);
		}
	 }
	ReturnMsg.GPGLL = GetGPGLL(pRingBuf);

	return ReturnMsg;
}
