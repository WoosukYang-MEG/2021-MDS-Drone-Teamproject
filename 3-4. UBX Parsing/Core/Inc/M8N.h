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
