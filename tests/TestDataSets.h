/*
 * TestDataSets.h
 *
 *  Created on: 06/05/2023
 *      Author: Utilizador
 */

#ifndef TESTDATASETS_H_
#define TESTDATASETS_H_


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

static char Neo6mTrackingData1SVSet[] =
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
		"$GPGSV,1,1,06,23,30,053,47,27,69,030,38*7AZZ"
		"$GPGLL,2769.50696,N,00321.52135,W,140722.00,A,A*74ZZ"
		"$GPRMC,140722.00,A,2769.50696,N,0091";

static char Neo6mTrackingUnexpectedDataSet[] =
		"$GPVTG,T,,M,0.196,N,0.364,K,A*2CZZ"
		"$GPGGA,140721.00,2769.50673,N,00321.52120,W,1,05,5.79,130.4,M,48.3,M,,*4AZZ"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CZZ"
		"$GPGSV,2,1,06,02,30,050,44,08,,,28,10,48,093,40,18,06,050,43*44ZZ"
		"$GPGSV,2,2,06,23,30,053,47,27,69,030,38*7AZZ"
		"$GPGLL,2769.50673,N,00321.52120,W,140721.00,A,A*78ZZ"
		"$GPRMC,140722.00,A,2769.50696,N,00321.52135,W,0.219,,290423,,,A*69ZZ"
		"$GPVTG,,T,,0.69420,0.219,N,0.405,K,A*28ZZ" //<- Expected data is in this line, after T
		"$GPGGA,140722.00,2769.50696,N,00321.52135,W,1,05,5.79,130.5,M,48.3,M,,*47ZZ"
		"$GPGSA,A,3,18,23,10,02,27,,,,,,,,7.48,5.79,4.73*0CZZ"
		"$GPGSV,1,1,06,23,30,053,47,27,69,030,38*7AZZ"
		"$GPGLL,2769.50696,N,00321.52135,W,140722.00,A,A*74ZZ"
		"$GPRMC,140722.00,A,2769.50696,N,0091";

static Neo6mDefaultMsg_t ExpectTemplate =
{
	.GPRMC =
	{
		.UtcTime =
		{
				.Hours = 0,
				.Minutes = 0,
				.Seconds = 0,
		},
		.Status = 0,
		.Latitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.NS = 0,
		.Longitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.EW = 0,
		.Speed = 0,
		.TrackMadeGood=0,
		.Date =
		{
				.Year = 0,
				.Month = 0,
				.Day = 0,
		},
		.MagneticVariation = 0,
		.EW_MV = 0,
		.DataStatus = 0,

	},
	.GPVTG =
	{
			.TrueTrackDegrees = 0,
			.MagneticTrackDegrees = 0,
			.SpeedKnots = 0,
			.SpeedKph = 0,
			.DataStatus = 0,
	},
	.GPGGA =
	{
		.UtcTime =
		{
				.Hours = 0,
				.Minutes = 0,
				.Seconds = 0,
		},
		.Latitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.NS = 0,
		.Longitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.EW = 0,
		.GpsQuality = 0,
		.SatsInView = 0,
		.HDOP = 0,
		.AntennaAltitude = 0,
		.GeoIdalSeparation = 0,
		.GpsDataAge = 0,
		.RefStationId = 0,
	},
	.GPGSA =
	{
		.ModeMA = 0,
		.Mode123 = 0,
		.PRN = {0,0,0,0,0,0,0,0,0,0,0,0},
		.PDOP = 0,
		.HDOP = 0,
		.VDOP = 0,
	},
	.GPGSV =
	{
		{
			.GPGSVSentences = 0,
			.SentenceIndex = 0,
			.SatsInView = 0,
			.SatInfo =
			{
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
			},
		},
		{
			.GPGSVSentences = 0,
			.SentenceIndex = 0,
			.SatsInView = 0,
			.SatInfo =
			{
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
			},
		},

	},
	.GPGLL =
	{
		.Latitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.NS = 0,
		.Longitude =
		{
				.Degrees = 0,
				.DecimalMinutes = 0,
		},
		.EW = 0,
		.UtcTime =
		{
				.Hours = 0,
				.Minutes = 0,
				.Seconds = 0,
		},
		.DataStatus = 0,
		.FAAModeIndicator = 0
	},
};


static Neo6mDefaultMsg_t ExpectDefaultMsg_TrackingData =
{
	.GPRMC =
	{
		.UtcTime =
		{
				.Hours = 14,
				.Minutes = 07,
				.Seconds = 22,
		},
		.Status = 'A',
		.Latitude =
		{
				.Degrees = 27,
				.DecimalMinutes = 69.50696,
		},
		.NS = 'N',
		.Longitude =
		{
				.Degrees = 3,
				.DecimalMinutes = 21.52135,
		},
		.EW = 'W',
		.Speed = 0.219,
		.TrackMadeGood=FLOAT_NOT_FOUND,
		.Date =
		{
				.Year = 2023,
				.Month = 04,
				.Day = 23,
		},
		.MagneticVariation = FLOAT_NOT_FOUND,
		.EW_MV = CHAR_NOT_FOUND,
		.DataStatus = 'A',

	},
	.GPVTG =
	{
			.TrueTrackDegrees = FLOAT_NOT_FOUND,
			.MagneticTrackDegrees = FLOAT_NOT_FOUND,
			.SpeedKnots = 0.219,
			.SpeedKph = 0.405,
			.DataStatus = 'A',
	},
	.GPGGA =
	{
		.UtcTime =
		{
				.Hours = 14,
				.Minutes = 7,
				.Seconds = 22,
		},
		.Latitude =
		{
				.Degrees = 27,
				.DecimalMinutes = 69.50696,
		},
		.NS = 'N',
		.Longitude =
		{
				.Degrees = 3,
				.DecimalMinutes = 21.52135,
		},
		.EW = 'W',
		.GpsQuality = 1,
		.SatsInView = 5,
		.HDOP = 5.79,
		.AntennaAltitude = 130.5,
		.GeoIdalSeparation = 48.3,
		.GpsDataAge = FLOAT_NOT_FOUND,
		.RefStationId = UINT16_NOT_FOUND,
	},
	.GPGSA =
	{
		.ModeMA = 'A',
		.Mode123 = '3',
		.PRN = {18,23,10,02,27,UINT16_NOT_FOUND,UINT16_NOT_FOUND,UINT16_NOT_FOUND,UINT16_NOT_FOUND,UINT16_NOT_FOUND,UINT16_NOT_FOUND,UINT16_NOT_FOUND},
		.PDOP = 7.48,
		.HDOP = 5.79,
		.VDOP = 4.73,
	},
	.GPGSV =
	{
		{
			.GPGSVSentences = 2,
			.SentenceIndex = 1,
			.SatsInView = 6,
			.SatInfo =
			{
				{.PRN = 2,.Elevation = 30,.Azimuth = 50,.SNR = 44},
				{.PRN = 8,.Elevation = UINT16_NOT_FOUND,.Azimuth = UINT16_NOT_FOUND,.SNR = 28},
				{.PRN = 10,.Elevation = 48,.Azimuth = 93,.SNR = 40},
				{.PRN = 18,.Elevation = 6,.Azimuth = 50,.SNR = 43},
			},
		},
		{
			.GPGSVSentences = 2,
			.SentenceIndex = 2,
			.SatsInView = 6,
			.SatInfo =
			{
				{.PRN = 23,.Elevation = 30,.Azimuth = 53,.SNR = 47},
				{.PRN = 27,.Elevation = 69,.Azimuth = 30,.SNR = 38},
				{.PRN = UINT16_NOT_FOUND,.Elevation = UINT16_NOT_FOUND,.Azimuth = UINT16_NOT_FOUND,.SNR = UINT16_NOT_FOUND},
				{.PRN = UINT16_NOT_FOUND,.Elevation = UINT16_NOT_FOUND,.Azimuth = UINT16_NOT_FOUND,.SNR = UINT16_NOT_FOUND},
			},
		},

	},
	.GPGLL =
	{
		.Latitude =
		{
			.Degrees = 27,
			.DecimalMinutes = 69.50696,
		},
		.NS = 'N',
		.Longitude =
		{
			.Degrees = 3,
			.DecimalMinutes = 21.52135,
		},
		.EW = 'W',
		.UtcTime =
		{
				.Hours = 14,
				.Minutes = 7,
				.Seconds = 22,
		},
		.DataStatus = 'A',
		.FAAModeIndicator = 'A'
	},
};

//expected struct with non tracking data
//expected struct with second (incomplete) tracking data
//expected struct with 1 sv tracking data
//expected struct with unexpected data



static char TooLargeDataSet[] =
		"$GPVTG,T,,M,0.196293892328372733,12345678937228,";
static char NonNumericDataSet[] =
		"$GPVTG,T,,M,0.1962.38929,51LA,";


#endif /* TESTDATASETS_H_ */