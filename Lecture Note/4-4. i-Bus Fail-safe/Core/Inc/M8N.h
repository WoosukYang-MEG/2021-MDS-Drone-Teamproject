#ifndef __M8N_H
#define __M8N_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

typedef struct _M8N_UBX_NAV_POLISH{
	unsigned char CLASS;
	unsigned char ID;
	unsigned char LENGTH;

	unsigned char iTOW;
	signed int lon;
	signed int lat;
	signed int height;
	signed int hMSL;
	unsigned char hAcc;
	unsigned char vAcc;

	double lon_f63;
	double lat_f63;
}M8N_UBX_NAV_POLISH;

extern M8N_UBX_NAV_POLISH polish;

unsigned char M8N_UBX_CHKSUM_Check(unsigned char* data, unsigned char len);
void M8N_UBX_POSLLH_Parsing(unsigned char* data, M8N_UBX_NAV_POLISH* polish);
void M8N_TransmitData(unsigned char* data, unsigned char len);
void M8N_UART4_Initialization(void);
void M8N_Initialization(void);

#ifdef __cplusplus
}
#endif
#endif /*__ M8N_H */
