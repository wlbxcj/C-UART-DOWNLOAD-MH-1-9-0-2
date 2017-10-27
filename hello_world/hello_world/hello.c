/*-------------------------------------------------------
      Data Encryption Standard  56位密钥加密64位数据 
                  2011.10
--------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "com.h"
#include <fcntl.h>
#include <process.h>
#include <sys/stat.h>
#include "scom.h"
#include "download.h"

int main(void)
{
	unsigned int nbyte, step = DOWNLOAD_STEP_ONE;
	FILE *fp;

    open_com();

	while (1)
	{
        printf("step %c\r\n", step);
		switch (step)
		{
		case DOWNLOAD_STEP_ONE:
			step = DOWNLOAD_STEP_ONE;
			if (download_step_one() == 0)
				step = DOWNLOAD_STEP_TWO;
			break;
		
		case DOWNLOAD_STEP_TWO:
			step = DOWNLOAD_STEP_ONE;
			if (download_step_two() == 0)
				step = DOWNLOAD_STEP_THR;
			break;
		
		case DOWNLOAD_STEP_THR:
			step = DOWNLOAD_STEP_ONE;
			if (download_step_three() == 0)
				step = DOWNLOAD_STEP_FOR;
			break;
		
		case DOWNLOAD_STEP_FOR:
            step = DOWNLOAD_STEP_ONE;
            if (download_step_four() == 0)
                step = DOWNLOAD_STEP_FIV;
            break;

		case DOWNLOAD_STEP_FIV:
            step = DOWNLOAD_STEP_ONE;
            if (download_step_five() == 0)
            {
                printf("下载成功!\r\n");
                scanf("%d,",&nbyte);
            }
            return;

		default:
			step = DOWNLOAD_STEP_ONE;
			break;
		}
		
	}
	while(1);
}
