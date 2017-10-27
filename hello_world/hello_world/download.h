#ifndef __DOWN_LOAD_H__
#define	__DOWN_LOAD_H__

enum
{
	DOWNLOAD_STEP_ONE = 0x31,
	DOWNLOAD_STEP_TWO = 0x32,
	DOWNLOAD_STEP_THR = 0x33,
	DOWNLOAD_STEP_FOR = 0x34,
	DOWNLOAD_STEP_FIV = 0x35,
};

enum
{
	TEST_STAGES     = 0x02,
	DEBUG_STAGES    = 0x03,
	PRODUCT_STAGES  = 0x04,
	FINISH_STAGES   = 0x05,
};


#define START_ADDR              (0x1001000)

#define FILE_VALID_FLAG			0x5555AAAAL
#define FLASH_BASE_ADDR			0x01000000L
#define FLASH_SIZE				(1 * 1024 * 1024)
#define FLASH_PROTECT_UNIT		(32 * 1024)
#define FLASH_SECTOR_SIZE		(4 * 1024)

void Crc16CCITT(const unsigned char *pbyDataIn, int dwDataLen, unsigned char abyCrcOut[2]);

int download_step_one(void);
int download_step_two(void);
int download_step_three(void);
int download_step_four(void);
int download_step_five(void);

#endif
