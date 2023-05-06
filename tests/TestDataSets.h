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


static Neo6mDefaultMsg_t ExpectDefaultMsg_TrackingData =
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
	},
	.GPGSA =
	{
		.ModeMA = 0,
		.Mode123 = 0,
		.PRN = {0,0,0,0,0,0,0,0,0,0,0,0},
		.PDOP = 0,
		.HDOP = 0,
		.VDOP = 0,
		.SysId = 0,
	},
	.GPGSV =
	{
		{
			.GPGSVSentences = 0,
			.SentenceIndex = 0,
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
			.SatInfo =
			{
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
				{.PRN = 0,.Elevation = 0,.Azimuth = 0,.SNR = 0},
			},
		},

	},

	.GPVTG =
	{
			.TrueTrackDegrees = 0,
			.MagneticTrackDegrees = 0,
			.SpeedKnots = 0,
			.SpeedKph = 0,
			.DataStatus = 0,
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
