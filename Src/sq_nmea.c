/*
 * Copyright (C) 2017 Konstantin Burdinov <sainquake@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "sq_nmea.h"
#include<stdio.h>
#include <stdlib.h>
#include <math.h>

buffer_start_end_t findSentence(uint8_t start){
	buffer_start_end_t o;
	for(int i=start;i<256;i++){
		if(GNS.buffer[i]=='$'){
			o.start = i;
		}
		if(GNS.buffer[i]==0x0D){
			o.end = i;
		}
		break;
	}
	ses = o;
	return o;
}

/*uint8_t findStart(void){

}*/
/*
void getCommand(void){
	for(int  j=0;j<32;j++){
		if(GNS.buffer[ ses.start + j ] == ',' ){
			ses.start += j;
			break;
		}
		GNS.command[j] = GNS.buffer[ses.start];
	}
}
void getData(void){
	for(int k = ses.start ; k < ses.end ; k++){
		GNS.data[ k-ses.start ] = GNS.buffer[k];
	}
}
void bufferToCommandAndData(void){
	if(GNS.buffer_updated){

	}
}
*/
char *strtok_fr (char *s, char delim, char **save_ptr)
{
    char *tail;
    char c;

    if (s == NULL) {
        s = *save_ptr;
    }
    tail = s;
    if ((c = *tail) == '\0') {
        s = NULL;
    }
    else {
        do {
            if (c == delim) {
                *tail++ = '\0';
                break;
           }
        }while ((c = *++tail) != '\0');
    }
    *save_ptr = tail;
    return s;
}

char *strtok_f (char *s, char delim)
{
    static char *save_ptr;

    return strtok_fr (s, delim, &save_ptr);
}

void parseRMC(void){
	char* Message_ID = strtok_f(RMC_data.sentence,',');
    char* Time = strtok_f(NULL,',');
    char* Data_Valid = strtok_f(NULL,',');
    char* Raw_Latitude = strtok_f(NULL,',');
    char* N_S = strtok_f(NULL,',');
    char* Raw_Longitude = strtok_f(NULL,',');
    char* E_W = strtok_f(NULL,',');
    char* Speed = strtok_f(NULL,',');
    char* COG = strtok_f(NULL,',');
    char* Date = strtok_f(NULL,',');
    char* Magnetic_Variation = strtok_f(NULL,',');
    char* M_E_W = strtok_f(NULL,',');
    char* Positioning_Mode = strtok_f(NULL,',');

    //Fix2.status = STATUS_NO_FIX;
    //float fTime = atof(Time);
    //if(fTime>0)
    //	Fix2.status = STATUS_TIME_ONLY;

    float Latitude = atof(Raw_Latitude);
    float latitude_deg_1e8 = round(Latitude/100.0);
    latitude_deg_1e8 = (Latitude-latitude_deg_1e8*100.0)/60 + latitude_deg_1e8;
    latitude_deg_1e8 = latitude_deg_1e8*100000000;

    float Longitude = atof(Raw_Longitude);
    float longitude_deg_1e8 = round(Longitude/100.0);
    longitude_deg_1e8 = (Longitude-longitude_deg_1e8*100.0)/60 + longitude_deg_1e8;
    longitude_deg_1e8 = longitude_deg_1e8*100000000;

    //if((latitude_deg_1e8!=0) && (longitude_deg_1e8 !=0))
    //	Fix2.status = STATUS_2D_FIX;

    Fix2.longitude_deg_1e8 = longitude_deg_1e8;
    Fix2.latitude_deg_1e8 = latitude_deg_1e8;

    float speed = atof(Speed);
    speed = speed*0.514444; //m/s
    float cog = atof(COG);
    cog = cog/180.0*3.14159265;//rad

    float vel_n = speed*(float)cos((double)(cog));
    float vel_e = speed*(float)sin((double)(cog));
    float vel_d = 0;

    Fix2.ned_velocity[0] = vel_n;
    Fix2.ned_velocity[1] = vel_e;
    Fix2.ned_velocity[2] = vel_d;
/*
    if(altitude!=0)
    	Fix2.status = STATUS_3D_FIX;

	float geo = atof(Geoidal);
	geo = geo*1000.0;

	Fix2.height_msl_mm = -altitude;
	Fix2.height_ellipsoid_mm = -geo;

	uint8_t sat_num = atoi(Number_of_satellites_in_view);
	Fix2.sats_used = sat_num;
*/
	//float pdop = atof(Horizontal_Dilution_of_precision);
	//Fix2.pdop = pdop;

	GGA_data.release = 1;

}
void parseGGA(void){
	char* Message_ID = strtok_f(GGA_data.sentence,',');
    char* Time = strtok_f(NULL,',');
    //char* Data_Valid = strtok_f(NULL,',');
    char* Raw_Latitude = strtok_f(NULL,',');
    char* N_S = strtok_f(NULL,',');
    char* Raw_Longitude = strtok_f(NULL,',');
    char* E_W = strtok_f(NULL,',');
    char* GPS_Quality_Indicator = strtok_f(NULL,',');
    char* Number_of_satellites_in_view = strtok_f(NULL,',');
    char* Horizontal_Dilution_of_precision = strtok_f(NULL,',');
    char* Altitude = strtok_f(NULL,',');
    char* Units_Altitude = strtok_f(NULL,',');
    char* Geoidal = strtok_f(NULL,',');
    char* Units_Geoidal = strtok_f(NULL,',');
    char* Age = strtok_f(NULL,',');
    char* Differential_reference_station_ID = strtok_f(NULL,',');

    Fix2.status = STATUS_NO_FIX;
    float fTime = atof(Time);
    if(fTime>0)
    	Fix2.status = STATUS_TIME_ONLY;

    float Latitude = atof(Raw_Latitude);
    float latitude_deg_1e8 = round(Latitude/100.0);
    latitude_deg_1e8 = (Latitude-latitude_deg_1e8*100.0)/60 + latitude_deg_1e8;
    latitude_deg_1e8 = latitude_deg_1e8*100000000;

    float Longitude = atof(Raw_Longitude);
    float longitude_deg_1e8 = round(Longitude/100.0);
    longitude_deg_1e8 = (Longitude-longitude_deg_1e8*100.0)/60 + longitude_deg_1e8;
    longitude_deg_1e8 = longitude_deg_1e8*100000000;

    if((latitude_deg_1e8!=0) && (longitude_deg_1e8 !=0))
    	Fix2.status = STATUS_2D_FIX;

    Fix2.longitude_deg_1e8 = longitude_deg_1e8;
    Fix2.latitude_deg_1e8 = latitude_deg_1e8;

    float altitude = atof(Altitude);
    altitude = altitude*1000.0;


    if(altitude!=0)
    	Fix2.status = STATUS_3D_FIX;

	float geo = atof(Geoidal);
	geo = geo*1000.0;

	Fix2.height_msl_mm = altitude;
	Fix2.height_ellipsoid_mm = geo;

	uint8_t sat_num = atoi(Number_of_satellites_in_view);
	Fix2.sats_used = sat_num;

	float pdop = atof(Horizontal_Dilution_of_precision);
	Fix2.pdop = pdop;

	GGA_data.release = 1;
	//Fix2.ned_velocity[0];
	//0,514444
}

void dataToFix2(void){
	//char str[] = GNS.sentence[RMC].data;
		char* Message_ID = strtok_f(RMC_data.sentence,',');
	    char* Time = strtok_f(NULL,',');
	    char* Data_Valid = strtok_f(NULL,',');
	    char* Raw_Latitude = strtok_f(NULL,',');
	    char* N_S = strtok_f(NULL,',');
	    char* Raw_Longitude = strtok_f(NULL,',');
	    char* E_W = strtok_f(NULL,',');
	    char* Speed = strtok_f(NULL,',');
	    char* COG = strtok_f(NULL,',');
	    char* Date = strtok_f(NULL,',');
	    char* Magnetic_Variation = strtok_f(NULL,',');
	    char* M_E_W = strtok_f(NULL,',');
	    char* Positioning_Mode = strtok_f(NULL,',');

	    float fTime = atof(Time);


	    float Latitude = atof(Raw_Latitude);
	    float latitude_deg_1e8 = round(Latitude/100.0);
	    latitude_deg_1e8 = (Latitude-latitude_deg_1e8*100.0)/60 + latitude_deg_1e8;
	    latitude_deg_1e8 = latitude_deg_1e8*100000000;
	    float Longitude = atof(Raw_Longitude);
	    float longitude_deg_1e8 = round(Longitude/100.0);
	    longitude_deg_1e8 = (Longitude-longitude_deg_1e8*100.0)/60 + longitude_deg_1e8;
	    longitude_deg_1e8 = longitude_deg_1e8*100000000;

	    Fix2.longitude_deg_1e8 = longitude_deg_1e8;
	    Fix2.latitude_deg_1e8 = latitude_deg_1e8;

	    char iiii=1;
	//GNS.sentence[RMC].data;
	/*int count = 0;
	int pos=0;
	for(int  j=0; j<128; j++){
		if (count==0){

		}
		//char* time = GNS.sentence[RMC].data;
		//RMC_data.time = time;
		//char* valid = FIND_AND_NUL(time, valid, ',');

		//RMC_data.valid = valid;
	}*/

}
