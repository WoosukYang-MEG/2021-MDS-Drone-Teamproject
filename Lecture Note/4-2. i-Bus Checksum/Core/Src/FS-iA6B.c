/*
 * FS-iA6B.c
 *
 *  Created on: Sep 5, 2021
 *      Author: TEST01
 */

#include "FS-iA6B.h"

unsigned char iBus_Check_CHKSUM(unsigned char* data, unsigned char len){
	unsigned short chksum=0xffff;

	for(int i=0; i<len-2; i++){
		chksum=chksum-data[i];
	}

	return ((chksum&0x00ff)==data[30]) && ((chksum>>8)==data[31]);
}
