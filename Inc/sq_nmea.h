/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef INC_SQ_NMEA_H_
#define INC_SQ_NMEA_H_

#include <stdint.h>

#define FIND_AND_NUL(s, p, c) ( \
   (p) = strchr(s, c), \
   *(p) = '\0', \
   ++(p), \
   (p))

typedef enum {
	H_UNKNOWN,
	GGA, // -  данные о последнем определении местоположения
	GLL, // -  координаты, широта/долгота
	GSA, // -  DOP (GPS) и активные спутники
	GSV, // -  наблюдаемые спутники
	WPL, // -  параметры заданной точки
	BOD, // -  азимут одной точки относительно другой
	RMB, // -  рекомендуемый минимум навигационных данных для достижения заданной точки
	RMC, // -  рекомендуемый минимум навигационных данных (см. выше)
	RTE // -  маршруты
} NMEA_header_t;


//«GL» — ГЛОНАСС, «GA» — Галилео, «GN» — ГЛОНАСС+GPS и т. п.

typedef enum {
	S_UNKNOWN,
	GP, // GPS
	GL, //«GL» — ГЛОНАСС
	GA, //«GA» — Галилео, «GN» — ГЛОНАСС+GPS и т. п.
	GN, //«GN» — ГЛОНАСС+GPS и т. п.
} NMEA_source_t;

typedef struct
{
	NMEA_source_t source;
	NMEA_header_t header;
	uint8_t data[128];

} NMEA_sentence_t;

typedef struct
{
	uint8_t buffer[256];
	//uint8_t command[32];
	//uint8_t data[128];
	//uint8_t buffer_updated;
	//NMEA_sentence_t sentence[20];

} NMEA_common_t;

NMEA_common_t GNS;

typedef struct
{
	uint8_t start;
	uint8_t end;
} buffer_start_end_t;

buffer_start_end_t ses;

void getCommand(void);

buffer_start_end_t findSentence(uint8_t start);


void bufferToCommandAndData(void);

typedef struct
{
	char sentence[128];
	char end;
	uint8_t release;
} sentence_t;

sentence_t RMC_data;
sentence_t GGA_data;
sentence_t RMF_data;

/*
1 2 3 4 5 6 7 8 9 10 11|
| | | | | | | | | | | |
$--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh
1) Time (UTC)
2) Status, V = Navigation receiver warning
3) Latitude
4) N or S
5) Longitude
6) E or W
7) Speed over ground, knots
8) Track made good, degrees true
9) Date, ddmmyy
10) Magnetic Variation, degrees
11) E or W
12) Checksum
*/

typedef struct
{
	//,091106.00,A,5545.17828,N,04844.65973,E,0.339,,090620,,,A*62
	char sentence[128];
	char end;
	char time[9];
	char valid;
	char Latitude[10];
	char N_S;
	char Longitude[11];
	char E_W;
	char vel[20];
	char dir[20];
	char date[6];
	char mag[20];
	char mE_W;
	char m;
	char crc;
} RMC_t;
//RMC_t RMC_data;

/*
1 2 3 4 5 6 7 8 9 10 | 12 13 14 15
| | | | | | | | | | | | | | |
$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
1) Time (UTC)
2) Latitude
3) N or S (North or South)
4) Longitude
5) E or W (East or West)
6) GPS Quality Indicator,
0 - fix not available,
1 - GPS fix,
2 - Differential GPS fix
7) Number of satellites in view, 00 - 12
8) Horizontal Dilution of precision
9) Antenna Altitude above/below mean-sea-level (geoid)
10) Units of antenna altitude, meters
11) Geoidal separation, the difference between the WGS-84 earth
ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
12) Units of geoidal separation, meters
13) Age of differential GPS data, time in seconds since last SC104
type 1 or 9 update, null field when DGPS is not used
14) Differential reference station ID, 0000-1023
15) Checksum
*/

typedef struct
{
	//,091106.00,A,5545.17828,N,04844.65973,E,0.339,,090620,,,A*62
	char sentence[128];
	char end;
	uint8_t release;
	char time[9];
	char valid;
	char Latitude[10];
	char N_S;
	char Longitude[11];
	char E_W;
	char Quality;
	char SatNum[2];
	char precision[10];
	char Altitude[10];
	char unitsA;
	char mean_sea_level[10];
	char unitsMSL;
	char age;
	char id[4];
	char crc;
} GGA_t;
//GGA_t GGA_data;

#define STATUS_NO_FIX      0
#define STATUS_TIME_ONLY   1
#define STATUS_2D_FIX      2
#define STATUS_3D_FIX      3

typedef struct
{
	uint64_t timestamp;
	uint64_t gnss_timestamp;
	uint8_t gnss_time_standard;
	uint8_t num_leap_seconds;
	int64_t longitude_deg_1e8;
	int64_t latitude_deg_1e8;
	int32_t height_ellipsoid_mm;
	int32_t height_msl_mm;
	float ned_velocity[3];
	uint8_t sats_used;
	uint8_t status;
	uint8_t mode;
	uint8_t sub_mode;
	uint16_t covariance[36];
	float pdop;
} fix2_t;

//uint8_t GNS_data_received=0;
fix2_t Fix2;

void parseRMC(void);
void parseGGA(void);

void dataToFix2(void);
/*
uint8_t time[10];
uint8_t GNSSstatus;
uint8_t GNSSlatitude[7];//широта
uint8_t GNSSlongitude[7];//догота
*/
#endif /* INC_SQ_NMEA_H_ */
