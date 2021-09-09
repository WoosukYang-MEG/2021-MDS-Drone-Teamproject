/*
 * M8N.c
 *
 *  Created on: Sep 5, 2021
 *      Author: TEST01
 */
#include "M8N.h"

M8N_UBX_NAV_POLISH polish;

const unsigned char UBX_CFG_RPT[]={
		0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00,
		0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9A, 0x79
};

const unsigned char UBX_CFG_MSG[]={
		0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x13, 0xBE
};

const unsigned char UBX_CFG_RATE[]={
		0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00,
		0x01, 0x00, 0xDE, 0x6A
};

const unsigned char UBX_CFG_CFG[]={
		0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x31,
		0xBF
};

void M8N_TransmitData(unsigned char* data, unsigned char len){
	for(int i=0; i<len; i++){
		while(!LL_USART_IsActiveFlag_TXE(UART4));
		LL_USART_TransmitData8(UART4, *(data+i));
	}
}

void M8N_UART4_Initialization(void){
	LL_USART_InitTypeDef USART_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	/**UART4 GPIO Configuration
	PC10   ------> UART4_TX
	PC11   ------> UART4_RX
	*/
	GPIO_InitStruct.Pin = MBN_TX4_Pin|MBN_RX4_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* UART4 interrupt Init */
	NVIC_SetPriority(UART4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(UART4_IRQn);

	USART_InitStruct.BaudRate = 9600;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(UART4, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(UART4);
	LL_USART_Enable(UART4);
}

void M8N_Initialization(void){
	M8N_UART4_Initialization();

	M8N_TransmitData(UBX_CFG_RPT, sizeof(UBX_CFG_RPT));
	HAL_Delay(100);
	M8N_TransmitData(UBX_CFG_MSG, sizeof(UBX_CFG_MSG));
	HAL_Delay(100);
	M8N_TransmitData(UBX_CFG_RATE, sizeof(UBX_CFG_RATE));
	HAL_Delay(100);
	M8N_TransmitData(UBX_CFG_CFG, sizeof(UBX_CFG_CFG));
}

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
