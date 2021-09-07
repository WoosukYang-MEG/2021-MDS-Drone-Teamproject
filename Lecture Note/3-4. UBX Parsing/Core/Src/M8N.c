/*
 * M8N.c
 *
 *  Created on: Sep 5, 2021
 *      Author: TEST01
 */
#include "M8N.h"

M8N_UBX_NAV_POLISH polish;

unsigned char M8N_UBX_CHKSUM_Check(unsigned char* data, unsigned char len){
	unsigned char CK_A=0;
	unsigned char CK_B=0;

	for(int i=2; i<len-2; i++){
		CK_A=CK_A+data[i];
		CK_B=CK_B+CK_A;
	}
	return (CK_A == data[len-2]) && (CK_B == data[len-1]);
}

void M8N_UBX_POSLLH_Parsing(unsigned char* data, M8N_UBX_NAV_POLISH* polish){
	polish->CLASS=data[2];
	polish->ID=data[3];
	polish->LENGTH=data[4] | data[5]<<8;		// little endian

	polish->iTOW=	data[6] | data[7]<<8 | data[8]<<16 | data[9]<<24;
	polish->lon= 	data[10] | data[11]<<8 | data[12]<<16 | data[13]<<24;
	polish->lat=	data[14] | data[15]<<8 | data[16]<<16 | data[17]<<24;
	polish->height=	data[18] | data[19]<<8 | data[20]<<16 | data[21]<<24;
	polish->hMSL=	data[22] | data[23]<<8 | data[24]<<16 | data[25]<<24;
	polish->hAcc=	data[26] | data[27]<<8 | data[28]<<16 | data[29]<<24;
	polish->vAcc=	data[30] | data[31]<<8 | data[32]<<16 | data[33]<<24;

//	polish->lon_f63=polish->lon/10000000.f;
//	polish->lat_f63=polish->lat/10000000.f;
}
