/*
 * Neo6m.h
 *
 *  Created on: May 1, 2023
 *      Author: Utilizador
 */

#ifndef NEO6MLITEFLEX_H_
#define NEO6MLITEFLEX_H_

#include "lwrb.h"

/*Changes the value of UT_STATIC depending on if UNIT_TEST is defined
 * Used to declare static functions in production code while making them visible to test
 * files */
#ifdef UNIT_TEST
  #define UT_STATIC
#else
  #define UT_STATIC static
#endif
/**
 *@brief Enumeration of all possible status code returned by functions in this library
 */
typedef enum
{
	NEO6M_SUCCESS,
	NEO6M_BUFFER_OVERFLOW,
	NEO6M_NULL_PTR,
	NEO6M_IO_ERROR,
} Neo6mLiteFlexStatus_t;

/**
 * @brief A type representing the signature that IO functions should have.
 *
 * @param1 Memory location where data is copied to
 * @param2 Number of bytes to be read
 *
 * @return Amount of bytes read
 */
typedef size_t (*IOFunc_t)(void*,size_t);

/**
 * @brief Amount of bytes that are read from the UART interface when the acquisition of a message is requested by the user.
 * This size correponds to an amount of bytes in which it is guaranteed that at least one full default neo6m message is present.
 */
#define NEO6M_BATCH_SIZE (750)
/**
 * @brief Extra size that the neo 6m msg buffer must have. Since it is not expected that all bytes in a batch are
 * interpreted immediately, a few of them can be left to be interpreted when acquisition is requested again. Thus the buffer needs
 * extra space to accomodate these leftover bytes
 */
#define NEO6M_BUFFER_SAFETY_MARGIN (200)
/**
 * @brief Size of the neo 6m message buffer. Size of defined batch + safety margin
 */
#define NEO6M_BATCH_BUFFER_SIZE NEO6M_BATCH_SIZE+NEO6M_BUFFER_SAFETY_MARGIN

/*Value to be returned by some functions when sequence could not be found*/
#define SEQUENCE_NOT_FOUND 0xFFFFFFFF
/*Value to be returned by some functions when a valid char could not be found*/
#define CHAR_NOT_FOUND 'X'
/*Value to be returned by some functions when a valid float could not be found*/
#define FLOAT_NOT_FOUND -9999999
/*Value to be returned by some functions when a valid float could not be found*/
#define UINT16_NOT_FOUND 0xFFFF
/**
 * @brief Abstract data type that acts as a handle to an Neo6mLiteFlex instance
 */
typedef struct Neo6mLiteFlexStruct* Neo6mLiteFlex_t;

typedef struct Time
{
	uint8_t Hours;
	uint8_t Minutes;
	float Seconds;
} Neo6mLiteFlex_Time_t;

typedef struct Date
{
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
} Neo6mLiteFlex_Date_t;

typedef struct DegDecMinutes
{
	uint8_t Degrees;
	float DecimalMinutes;
} Neo6mLiteFlex_DegDecMinutes_t;


typedef struct GPRMC
{
	Neo6mLiteFlex_Time_t UtcTime;
	char Status;
	Neo6mLiteFlex_DegDecMinutes_t Latitude;
	char NS;
	Neo6mLiteFlex_DegDecMinutes_t Longitude;
	char EW;
	float Speed;
	float TrackMadeGood;
	Neo6mLiteFlex_Date_t Date;
	float MagneticVariation;
	char EW_MV;
	char DataStatus;
} Neo6mLiteFlex_GPRMC_t;

typedef struct GPVTG
{
	float TrueTrackDegrees;
	float MagneticTrackDegrees;
	float SpeedKnots;
	float SpeedKph;
	char DataStatus;
} Neo6mLiteFlex_GPVTG_t;

typedef struct GPGGA
{
	Neo6mLiteFlex_Time_t UtcTime;
	Neo6mLiteFlex_DegDecMinutes_t Latitude;
	char NS;
	Neo6mLiteFlex_DegDecMinutes_t Longitude;
	char EW;
	float GpsQuality;
	uint16_t SatsInView;
	float HDOP;
	float AntennaAltitude;
	float GeoIdalSeparation;
	float GpsDataAge;
	uint16_t RefStationId;
} Neo6mLiteFlex_GPGGA_t;

typedef struct GPGSA
{
	char ModeMA;
	char Mode123;
	uint16_t PRN[12];
	float PDOP;
	float HDOP;
	float VDOP;
	uint16_t SysId;
} Neo6mLiteFlex_GPGSA_t;

typedef struct GPGSV_SatInfo
{
	uint16_t PRN;
	uint16_t Elevation;
	uint16_t Azimuth;
	uint16_t SNR;
} Neo6mLiteFlex_GPGSV_SatInfo_t;

typedef struct GPGSV
{
	uint16_t GPGSVSentences;
	uint16_t SentenceIndex;
	Neo6mLiteFlex_GPGSV_SatInfo_t SatInfo[4];
} Neo6mLiteFlex_GPGSV_t;

typedef struct GPGLL
{
	Neo6mLiteFlex_DegDecMinutes_t Latitude;
	char NS;
	Neo6mLiteFlex_DegDecMinutes_t Longitude;
	char EW;
	Neo6mLiteFlex_Time_t UtcTime;
	char DataStatus;
} Neo6mLiteFlex_GPGLL_t;

typedef struct Neo6mDefaultMsg
{
	Neo6mLiteFlex_GPRMC_t GPRMC;
	Neo6mLiteFlex_GPVTG_t GPVTG;
	Neo6mLiteFlex_GPGGA_t GPGGA;
	Neo6mLiteFlex_GPGSA_t GPGSA;
	Neo6mLiteFlex_GPGSV_t GPGSV[2];
	Neo6mLiteFlex_GPGLL_t GPGLL;
} Neo6mDefaultMsg_t;

Neo6mLiteFlex_t Neo6mLiteFlex_Create();
void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex);
void Neo6mLiteFlex_SetIORead(Neo6mLiteFlex_t Neo6mLiteFlex, IOFunc_t pIORead);
lwrb_t* Neo6mLiteFlex_GetRingBuffPtr(Neo6mLiteFlex_t Neo6mLiteFlex);
uint8_t* Neo6mLiteFlex_GetByteArray(Neo6mLiteFlex_t Neo6mLiteFlex);


#endif /* NEO6MLITEFLEX_H_ */
