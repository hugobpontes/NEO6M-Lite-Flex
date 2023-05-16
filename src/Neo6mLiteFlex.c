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
 * @brief Length of date data as written by NEO6M
 */
#define DATE_LENGTH 6
/**
 * @brief Length of utc time data as written by NEO6M
 */
#define TIME_LENGTH 9
/**
 * @brief Minimum Length of coordinate data as written by NEO6M
 */
#define COORDINATE_MIN_LENGTH 10
/**
 * @brief Maximum Length of coordinate data as written by NEO6M
 */
#define COORDINATE_MAX_LENGTH 11
/**
 * @brief If this expression is true, then next char read isnt a comma
 */
#define NEXT_CHAR_READ_ISNT_COMMA GetBytesUntilSequence(pRingBuf,",",1)
/**
 * @brief If this expression is true, then next char read isnt an asterisk
 */
#define NEXT_CHAR_READ_ISNT_ASTERISK GetBytesUntilSequence(pRingBuf,"*",1)
/**
 * @brief Macro defining the maximum number of chars that a string representing a float
 * can have, including the decimal point
 */
#define MAX_FLOAT_STRING_SIZE 15
/**
 * @brief Macro defining the maximum number of chars that a string representing an integer
 * can have
 */
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
 * @brief Returns number of times a given sequence repeats in the ring buffer
 *
 * @param pRingBuf Pointer to the ring buffer where chars to be found are stored
 * @param Sequence Sequence to with which chars in the ring buffer are compared
 * @param SequenceLength Length in bytes of the sequence parameter
 *
 * @return number of times a given sequence repeats in the ring buffer
 * or SEQUENCE_NOT_FOUND
 *
 */
static uint32_t GetSequenceRepeats(lwrb_t* pRingBuf, char* Sequence, uint32_t SequenceLength)
{
	uint32_t Repeats = 0;

	uint32_t PositionChecked = 0;

	uint32_t FullBytes = 0;

	if (Sequence && SequenceLength && (SequenceLength<MAX_SEQUENCE_SIZE) && pRingBuf )
	{
		FullBytes = lwrb_get_full(pRingBuf);
		if (SequenceLength < FullBytes)
		{
			/*Peek bytes equal to the size of the fed sequence until the end of the buffer and
			 * increase repeat count when there is a match*/
			for (PositionChecked =0;PositionChecked<(FullBytes-SequenceLength);PositionChecked++)
			{
				if(PeekIntoStringAndCompare(pRingBuf,Sequence,SequenceLength,PositionChecked))
				{
					Repeats++;
				}
			}

		}
	}

	if (!Repeats)
	{
		Repeats = SEQUENCE_NOT_FOUND;
	}

	return Repeats;
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
 * or SEQUENCE_NOT_FOUND
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
 * or SEQUENCE_NOT_FOUND
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
 * @return Char sitting immediately before the provided sequence or CHAR_NOT_FOUND
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
 * @return float number equivalent of provided char array or FLOAT_NOT_FOUND
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
 * 			or FLOAT_NOT_FOUND
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
/**
 * @brief Reads chars from a ring buffer and outputs the corresponding integer, if valid
 *
 * @param pRingBuf Pointer to the ring buffer where chars are stored
 * @param Number of chars of the integer string to be read
 *
 * @return Read chars converted into an integer or UINT16_NOT_FOUND
 *
 */
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
/**
 * @brief Converts chars between current position and provided sequence into a integer number.
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 * @param Sequence Sequence until which chars are converted to integer
 *
 * @return integer number equivalent of chars between current ring buffer position and sequence
 * 			or UINT16_NOT_FOUND
 */
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
/**
 * @brief Reads specified bytes from the ring buffer and convers them to an integer
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 * @param BytesToRead Number of bytes to be read and converted
 *
 * @return integer number equivalent corresponding to the next chars
 * 			or UINT16_NOT_FOUND
 */
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
/**
 * @brief Skips ring buffer until the end of the provided sequence
 * @param pRingBuf Pointer to the ring buffer where data chars are stored
 * @param Sequence Sequence until which the ring buffer is skipped
 *
 */
static void SkipUntilEndOfSequence(lwrb_t* pRingBuf, char* Sequence)
{
	uint32_t BytesToSkip;
	BytesToSkip = GetBytesUntilSequenceEnd(pRingBuf,Sequence);
	lwrb_skip(pRingBuf,BytesToSkip);
}
/**
 * @brief Reads time data from the current ring buffer position if its in NMEA 0813 format (as neo6m outputs)
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 *
 * @return time struct with read data or TIME_INIT
 */
static Neo6mLiteFlex_Time_t GetTime(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_Time_t ReturnTime = TIME_INIT;

	uint32_t BytesUntilComma = GetBytesUntilSequence(pRingBuf,",",1);

	if (BytesUntilComma != SEQUENCE_NOT_FOUND){
		if (BytesUntilComma == TIME_LENGTH)
		{
			ReturnTime.Hours = GetNextBytesAsInt(pRingBuf,2);
			ReturnTime.Minutes = GetNextBytesAsInt(pRingBuf,2);
			ReturnTime.Seconds = GetFloatUntilSequence(pRingBuf,",");
		}
		else
		{
			lwrb_skip(pRingBuf,BytesUntilComma+1);
		}
	}
	return ReturnTime;
}
/**
 * @brief Reads date data from the current ring buffer position if its in NMEA 0813 format (as neo6m outputs)
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 *
 * @return date struct with read data or DATE_INIT
 */
static Neo6mLiteFlex_Date_t GetDate(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_Date_t ReturnDate = DATE_INIT;

	uint32_t BytesUntilComma = GetBytesUntilSequence(pRingBuf,",",1);

	if (BytesUntilComma != SEQUENCE_NOT_FOUND){
		if (BytesUntilComma == DATE_LENGTH) /*Since several calls are made for a field without commas separating its elements, it is crucial
		to check if the field exists, otherwise we'd be reading outside the field alloted for the date */
		{
			ReturnDate.Day = GetNextBytesAsInt(pRingBuf,2);
			ReturnDate.Month = GetNextBytesAsInt(pRingBuf,2);
			ReturnDate.Year = GetNextBytesAsInt(pRingBuf,2);
			if (ReturnDate.Year != UINT16_NOT_FOUND)
			{
				ReturnDate.Year+= 2000; /*Sadly will only work until 2099 :( */
			}
		}
		else
		{
			lwrb_skip(pRingBuf,BytesUntilComma+1);
		}
	}
	return ReturnDate;
}
/**
 * @brief Reads coordinate (in degrees and decimals seconds format)
 * data from the current ring buffer position if its in NMEA 0813 format (as neo6m outputs)
 * @param pRingBuf Pointer to the ring buffer where chars to be converted are stored
 *
 * @return degree and decimal minutes struct with read data or COORDINATE_INIT
 */
static Neo6mLiteFlex_DegDecMinutes_t GetDegDecMinutes(lwrb_t* pRingBuf, uint8_t DegDigits)
{
	Neo6mLiteFlex_DegDecMinutes_t ReturnDegDecMinutes = COORDINATE_INIT;

	uint32_t BytesUntilComma = GetBytesUntilSequence(pRingBuf,",",1);

	if (BytesUntilComma != SEQUENCE_NOT_FOUND){
		if (BytesUntilComma >= COORDINATE_MIN_LENGTH && BytesUntilComma <= COORDINATE_MAX_LENGTH)
		{
			ReturnDegDecMinutes.Degrees = GetNextBytesAsInt(pRingBuf,DegDigits);
			ReturnDegDecMinutes.DecimalMinutes = GetFloatUntilSequence(pRingBuf,",");
		}
		else
		{
			lwrb_skip(pRingBuf,BytesUntilComma+1);
		}
	}

	return ReturnDegDecMinutes;
}
/**
 * @brief Reads Sat Info (PRN,Elevation,Azimuth,SNR) present in GPGSV messages
 * from the current ring buffer position if its in NMEA 0813 format (as neo6m outputs)
 * @param pRingBuf Pointer to the ring buffer where char data is
 *
 * @returns sat info struct with read data or SAT_INFO_INIT
 */
static Neo6mLiteFlex_GPGSV_SatInfo_t GetSatInfo(lwrb_t* pRingBuf, bool IsLastFlag)
{
	Neo6mLiteFlex_GPGSV_SatInfo_t ReturnSatInfo = SAT_INFO_INIT;

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
/**
 * @brief Reads full NMEA 0183 GPRMC message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 *
 * @returns GPRMC data struct or GPRMC_INIT
 */
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
/**
 * @brief Reads full NMEA 0183 GPVTG message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns GPVTG data struct or GPVTG_INIT
 */
static Neo6mLiteFlex_GPVTG_t GetGPVTG(lwrb_t* pRingBuf)
{
	Neo6mLiteFlex_GPVTG_t ReturnGPVTG = GPVTG_INIT;

	SkipUntilEndOfSequence(pRingBuf, "$GPVTG,");

	ReturnGPVTG.TrueTrackDegrees = GetFloatUntilSequence(pRingBuf,",");
	SkipUntilEndOfSequence(pRingBuf, ","); /*Ignore redundant char field*/
	ReturnGPVTG.MagneticTrackDegrees = GetFloatUntilSequence(pRingBuf,",");
	SkipUntilEndOfSequence(pRingBuf, ",");
	ReturnGPVTG.SpeedKnots = GetFloatUntilSequence(pRingBuf,",");
	SkipUntilEndOfSequence(pRingBuf, ",");
	ReturnGPVTG.SpeedKph = GetFloatUntilSequence(pRingBuf,",");
	SkipUntilEndOfSequence(pRingBuf, ",");
	ReturnGPVTG.DataStatus = GetCharBeforeSequence(pRingBuf,"*");

	return ReturnGPVTG;
}
/**
 * @brief Reads full NMEA 0183 GPGGA message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns GPGGA data struct or GPGGA_INIT
 */
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
	SkipUntilEndOfSequence(pRingBuf, ","); /*Skip until end of redundant char*/
	ReturnGPGGA.GeoIdalSeparation = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.GpsDataAge = GetFloatUntilSequence(pRingBuf, ",");
	ReturnGPGGA.RefStationId = GetIntUntilSequence(pRingBuf, "*");

	return ReturnGPGGA;
}
/**
 * @brief Reads full NMEA 0183 GPGSA message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns GPGSA data struct or GPGSA_INIT
 */
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
/**
 * @brief Reads full NMEA 0183 GPGSV message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns GPGSV data struct or GPGSV_INIT
 */
static Neo6mLiteFlex_GPGSV_t GetGPGSV(lwrb_t* pRingBuf, uint32_t* SatsParsed)
{
	Neo6mLiteFlex_GPGSV_t ReturnGPGSV = GPGSV_INIT;
	char Terminator[2] = {0};

	SkipUntilEndOfSequence(pRingBuf, "$GPGSV,");

	ReturnGPGSV.GPGSVSentences = GetIntUntilSequence(pRingBuf, ",");
	ReturnGPGSV.SentenceIndex = GetIntUntilSequence(pRingBuf, ",");
	lwrb_peek(pRingBuf,2,&Terminator,1); /*GPGSV might end earlier so we need to check what the terminator
	is for the sats in view field */
	ReturnGPGSV.SatsInView = GetIntUntilSequence(pRingBuf, Terminator);

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
/**
 * @brief Reads full NMEA 0183 GPGLL message from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns GPGLL data struct or GPGLL_INIT
 */
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
/**
 * @brief Reads full NEO6M default message (GPRMC+GPVTG+GPGGA,GPGSA,GPGSV,GPGLL)
 * from a ring buffer and returns a struct with the retrieved data
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns NEO 6M Default Message struct or NEO6M_MSG_INIT
 */
UT_STATIC Neo6mDefaultMsg_t GetDefaultMsg(lwrb_t* pRingBuf)
{
	Neo6mDefaultMsg_t ReturnMsg = NEO6M_MSG_INIT;

	uint32_t GPGSV_SatsParsed = 0;

	/*To make sure there is a GPRMC message, we look for the start of a GPVTG message, and so on*/
	if (GetBytesUntilSequence(pRingBuf, "$GPVTG,", 7) != SEQUENCE_NOT_FOUND)
	{
		ReturnMsg.GPRMC 	= GetGPRMC(pRingBuf);
	}
	if (GetBytesUntilSequence(pRingBuf, "$GPGGA,", 7) != SEQUENCE_NOT_FOUND)
	{
		ReturnMsg.GPVTG 	= GetGPVTG(pRingBuf);
	}
	if (GetBytesUntilSequence(pRingBuf, "$GPGSA,", 7) != SEQUENCE_NOT_FOUND)
	{
		ReturnMsg.GPGGA 	= GetGPGGA(pRingBuf);
	}
	if (GetBytesUntilSequence(pRingBuf, "$GPGSV,", 7) != SEQUENCE_NOT_FOUND)
	{
		ReturnMsg.GPGSA 	= GetGPGSA(pRingBuf);
	}
	if (GetBytesUntilSequence(pRingBuf, "$GPRMC,", 7) != SEQUENCE_NOT_FOUND)
	{
		ReturnMsg.GPGSV[0] 	= GetGPGSV(pRingBuf,&GPGSV_SatsParsed);
	}
	if (ReturnMsg.GPGSV[0].GPGSVSentences != UINT16_NOT_FOUND)
	{
		for (int idx = 1; idx<ReturnMsg.GPGSV[0].GPGSVSentences;idx++)
		{
			if (GetBytesUntilSequence(pRingBuf, "$GPRMC,", 7) != SEQUENCE_NOT_FOUND) {ReturnMsg.GPGSV[idx] 	= GetGPGSV(pRingBuf,&GPGSV_SatsParsed);}
		}
	}
	ReturnMsg.GPGLL = GetGPGLL(pRingBuf);

	return ReturnMsg;
}
/**
 * @brief Reads all NEO6M default messages (GPRMC+GPVTG+GPGGA,GPGSA,GPGSV,GPGLL) present in a ring buffer
 * into a provided default message array
 * @param pRingBuf Pointer to the ring buffer where chars data is.
 *
 * @returns Number of default messages read
 */
uint32_t Neo6mLiteFlex_GetNeo6mMsgs(Neo6mLiteFlex_t Neo6mLiteFlex,Neo6mMsgArray_t* Message)
{
	uint32_t MessagesRead = 0;
	uint32_t MessagesAvailable;

	lwrb_t* pRingBuf = &Neo6mLiteFlex->MessageRingBuffer;

	Neo6mLiteFlexStatus_t Status;

	Status = IOReadIntoRingBuffer(Neo6mLiteFlex,NEO6M_BATCH_SIZE);

	if (Status == NEO6M_SUCCESS)
	{
		MessagesAvailable = min(GetSequenceRepeats(pRingBuf,"$GPRMC,",7)-1,MAX_MSGS_IN_BATCH);
		for (MessagesRead=0; MessagesRead<MessagesAvailable;MessagesRead++)
		{
				(*Message)[MessagesRead] =  GetDefaultMsg(pRingBuf);
		}
	}

	return MessagesRead;
}
