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
/**
 * @brief Struct to hold utc time data as extracted from an NMEA 0183 message
 */
typedef struct Time
{
	uint16_t Hours;
	uint16_t Minutes;
	float Seconds;
} Neo6mLiteFlex_Time_t;
/**
 * @brief Struct to hold date data as extracted from an NMEA 0183 message
 */
typedef struct Date
{
	uint16_t Year;
	uint16_t Month;
	uint16_t Day;
} Neo6mLiteFlex_Date_t;
/**
 * @brief Struct to hold degree+decimal minutes data as extracted from an NMEA 0183 message
 */
typedef struct DegDecMinutes
{
	uint16_t Degrees;
	float DecimalMinutes;
} Neo6mLiteFlex_DegDecMinutes_t;
/**
 * @brief Struct to hold data present in an GPRMC NMEA 0183 message
 */
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
/**
 * @brief Struct to hold data present in an GPVTG NMEA 0183 message
 */
typedef struct GPVTG
{
	float TrueTrackDegrees;
	float MagneticTrackDegrees;
	float SpeedKnots;
	float SpeedKph;
	char DataStatus;
} Neo6mLiteFlex_GPVTG_t;
/**
 * @brief Struct to hold data present in an GPGGA NMEA 0183 message
 */
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
/**
 * @brief Struct to hold data present in an GPGSA NMEA 0183 message
 */
typedef struct GPGSA
{
	char ModeMA;
	char Mode123;
	uint16_t PRN[12];
	float PDOP;
	float HDOP;
	float VDOP;
} Neo6mLiteFlex_GPGSA_t;
/**
 * @brief Struct to hold "sat info" data (PRN,Elevation,Azimuth,SNR)
 */
typedef struct GPGSV_SatInfo
{
	uint16_t PRN;
	uint16_t Elevation;
	uint16_t Azimuth;
	uint16_t SNR;
} Neo6mLiteFlex_GPGSV_SatInfo_t;
/**
 * @brief Struct to hold data present in an GPGSV NMEA 0183 message
 */
typedef struct GPGSV
{
	uint16_t GPGSVSentences;
	uint16_t SentenceIndex;
	uint16_t SatsInView;
	Neo6mLiteFlex_GPGSV_SatInfo_t SatInfo[4];
} Neo6mLiteFlex_GPGSV_t;
/**
 * @brief Struct to hold data present in an GPGLL NMEA 0183 message
 */
typedef struct GPGLL
{
	Neo6mLiteFlex_DegDecMinutes_t Latitude;
	char NS;
	Neo6mLiteFlex_DegDecMinutes_t Longitude;
	char EW;
	Neo6mLiteFlex_Time_t UtcTime;
	char DataStatus;
	char FAAModeIndicator;
} Neo6mLiteFlex_GPGLL_t;
/**
 * @brief Struct to hold data present in the default data that NEO-6M outputs
 */
typedef struct Neo6mDefaultMsg
{
	Neo6mLiteFlex_GPRMC_t GPRMC;
	Neo6mLiteFlex_GPVTG_t GPVTG;
	Neo6mLiteFlex_GPGGA_t GPGGA;
	Neo6mLiteFlex_GPGSA_t GPGSA;
	Neo6mLiteFlex_GPGSV_t GPGSV[9];
	Neo6mLiteFlex_GPGLL_t GPGLL;
} Neo6mDefaultMsg_t;
/**
 * @brief Max messages that can be retrieved in a single call to GetNeo6mMsgs
 */
#define MAX_MSGS_IN_BATCH 4
/**
 * @brief Type for an array that holds MAX_MSGS_IN_BATCH default messages
 * (solely created to be passed to the GetNeo6mMsgs function)
 */
typedef Neo6mDefaultMsg_t Neo6mMsgArray_t[MAX_MSGS_IN_BATCH];

Neo6mLiteFlex_t Neo6mLiteFlex_Create();
void Neo6mLiteFlex_Destroy(Neo6mLiteFlex_t Neo6mLiteFlex);
void Neo6mLiteFlex_SetIORead(Neo6mLiteFlex_t Neo6mLiteFlex, IOFunc_t pIORead);
lwrb_t* Neo6mLiteFlex_GetRingBuffPtr(Neo6mLiteFlex_t Neo6mLiteFlex);
uint8_t* Neo6mLiteFlex_GetByteArray(Neo6mLiteFlex_t Neo6mLiteFlex);

uint32_t GetNeo6mMsgs(Neo6mLiteFlex_t Neo6mLiteFlex,Neo6mMsgArray_t* Message);

/* INITIAL/DEFAULT VALUES FOR THE STRUCTS DEFINED ABOVE */

#define TIME_INIT {\
    .Hours = UINT16_NOT_FOUND,\
    .Minutes = UINT16_NOT_FOUND,\
    .Seconds = FLOAT_NOT_FOUND\
}

#define LOCATION_INIT \
    .Latitude = COORDINATE_INIT,\
    .NS = CHAR_NOT_FOUND,\
    .Longitude = COORDINATE_INIT,\
    .EW = CHAR_NOT_FOUND,\

#define COORDINATE_INIT {\
    .Degrees = UINT16_NOT_FOUND,\
    .DecimalMinutes = FLOAT_NOT_FOUND\
}

#define DATE_INIT {\
    .Year = UINT16_NOT_FOUND,\
    .Month = UINT16_NOT_FOUND,\
    .Day = UINT16_NOT_FOUND\
}

#define PRN_INIT { \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND, \
    UINT16_NOT_FOUND \
}

#define SAT_INFO_INIT \
    {\
        .PRN = UINT16_NOT_FOUND,\
        .Elevation = UINT16_NOT_FOUND,\
        .Azimuth = UINT16_NOT_FOUND,\
        .SNR = UINT16_NOT_FOUND\
    }

#define GPGSV_INIT {\
    .GPGSVSentences = UINT16_NOT_FOUND,\
    .SentenceIndex = UINT16_NOT_FOUND,\
    .SatsInView = UINT16_NOT_FOUND,\
    .SatInfo = {\
        SAT_INFO_INIT,\
        SAT_INFO_INIT,\
        SAT_INFO_INIT,\
        SAT_INFO_INIT\
    }\
}

#define GPRMC_INIT { \
    .UtcTime = TIME_INIT, \
    .Status = CHAR_NOT_FOUND, \
    .Latitude = COORDINATE_INIT, \
    .NS = CHAR_NOT_FOUND, \
    .Longitude = COORDINATE_INIT, \
    .EW = CHAR_NOT_FOUND, \
    .Speed = FLOAT_NOT_FOUND, \
    .TrackMadeGood = FLOAT_NOT_FOUND, \
    .Date = DATE_INIT, \
    .MagneticVariation = FLOAT_NOT_FOUND, \
    .EW_MV = CHAR_NOT_FOUND, \
    .DataStatus = CHAR_NOT_FOUND \
}

#define GPVTG_INIT \
{ \
	.TrueTrackDegrees = FLOAT_NOT_FOUND, \
	.MagneticTrackDegrees = FLOAT_NOT_FOUND, \
	.SpeedKnots = FLOAT_NOT_FOUND, \
	.SpeedKph = FLOAT_NOT_FOUND, \
	.DataStatus = CHAR_NOT_FOUND, \
}

#define GPGGA_INIT {\
    .UtcTime = TIME_INIT,\
    .Latitude = COORDINATE_INIT,\
    .NS = CHAR_NOT_FOUND,\
    .Longitude = COORDINATE_INIT,\
    .EW = CHAR_NOT_FOUND,\
    .GpsQuality = FLOAT_NOT_FOUND,\
    .SatsInView = UINT16_NOT_FOUND,\
    .HDOP = FLOAT_NOT_FOUND,\
    .AntennaAltitude = FLOAT_NOT_FOUND,\
    .GeoIdalSeparation = FLOAT_NOT_FOUND,\
    .GpsDataAge = FLOAT_NOT_FOUND,\
    .RefStationId = UINT16_NOT_FOUND\
}

#define GPGSA_INIT {CHAR_NOT_FOUND, CHAR_NOT_FOUND, PRN_INIT, FLOAT_NOT_FOUND, FLOAT_NOT_FOUND, FLOAT_NOT_FOUND}

#define GPGLL_INIT \
    { \
        .Latitude = COORDINATE_INIT, \
        .NS = CHAR_NOT_FOUND, \
        .Longitude = COORDINATE_INIT, \
        .EW = CHAR_NOT_FOUND, \
        .UtcTime = TIME_INIT, \
        .DataStatus = CHAR_NOT_FOUND, \
        .FAAModeIndicator = CHAR_NOT_FOUND \
    }
#define NEO6M_MSG_INIT { \
	    .GPRMC = GPRMC_INIT, \
	    .GPVTG = GPVTG_INIT, \
	    .GPGGA = GPGGA_INIT, \
	    .GPGSA = GPGSA_INIT, \
	    .GPGSV = { \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	        GPGSV_INIT, \
	    }, \
	    .GPGLL = GPGLL_INIT, \
	}

#endif /* NEO6MLITEFLEX_H_ */
