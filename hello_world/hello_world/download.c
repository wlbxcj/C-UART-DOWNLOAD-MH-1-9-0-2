#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "com.h"
#include <fcntl.h>
#include <process.h>
#include <sys/stat.h>
#include <Windows.h> 
#include "download.h"
#include "newsha256.h"

#define HOS_TO_TDEVICE	0x01
#define DEVICE_TO_HOST	0xAA

#define STEP_ACK		0x28
#define STEP_NCK		0x29

unsigned char cur_life_stages = TEST_STAGES;  // 当前芯片的生命阶段，2 测试  3 调试  4 产品阶段


unsigned short g_awhalfCrc16CCITT[16]=
{ /* CRC 16bit余式表 */
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

static const unsigned int CRC32_Table[256] =
{
   0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
   0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
   0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
   0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
   0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
   0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
   0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
   0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
   0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
   0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
   0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
   0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
   0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
   0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
   0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
   0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
   0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
   0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
   0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
   0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
   0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
   0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
   0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
   0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
   0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
   0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
   0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
   0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
   0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
   0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
   0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
   0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
   0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
   0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
   0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
   0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
   0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
   0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
   0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
   0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
   0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
   0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
   0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
   0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
   0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
   0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
   0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
   0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
   0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
   0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
   0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
   0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
   0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
   0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
   0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
   0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
   0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
   0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
   0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
   0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
   0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
   0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
   0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
   0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};
unsigned int calculate_crc32 (void *pStart, unsigned int uSize)
{
#define INIT  0xffffffff
#define XOROT 0xffffffff

    unsigned int uCRCValue;
    unsigned char *pData;

    /* init the start value */
    uCRCValue = INIT;
    pData = pStart;

    /* calculate CRC */
    while (uSize --)
    {
        uCRCValue = CRC32_Table[(uCRCValue ^ *pData++) & 0xFF] ^ (uCRCValue >> 8);
    }
    /* XOR the output value */
    return uCRCValue ^ XOROT;
}

void Crc16CCITT(const unsigned char *pbyDataIn, int dwDataLen, unsigned char abyCrcOut[2])
{
    unsigned short wCrc = 0xffff;
    unsigned char byTemp;

    while (dwDataLen-- != 0)
    {
        byTemp = ((unsigned char)(wCrc>>8))>>4;
        wCrc <<= 4;
        wCrc ^= g_awhalfCrc16CCITT[byTemp^(*pbyDataIn/16)];
        byTemp = ((unsigned char)(wCrc>>8))>>4;
        wCrc <<= 4;
        wCrc ^= g_awhalfCrc16CCITT[byTemp^(*pbyDataIn&0x0f)];
        pbyDataIn++;
    }

    abyCrcOut[0] = wCrc/256;
    abyCrcOut[1] = wCrc%256;
}

int download_packet(unsigned char step,
					        unsigned int len,
					        unsigned char *scr,
					        unsigned char *dtr		// 打包后输出指针
					)
{
	unsigned int len_tmp = 0;
	unsigned char *p = dtr;
	unsigned char crc_buf[2];

	p[len_tmp++] = HOS_TO_TDEVICE;
	p[len_tmp++] = step;
	p[len_tmp++] = (len + 4);
	p[len_tmp++] = (len + 4) >> 8;
	memcpy(&p[len_tmp], scr, len);
	len_tmp += len;

	// crc16
	Crc16CCITT(p, len_tmp, crc_buf);
	p[len_tmp++] = crc_buf[1];
	p[len_tmp++] = crc_buf[0];

	return len_tmp;
}

int download_step_one(void)
{
#define STEP_ONE_REC_LEN    (29)
	unsigned int rec_len = 0, i = 0, j = 0;
	unsigned char send_buf[20];
	unsigned char rec_buf[200];
	
	clear_com();
	memset(send_buf, 0x7F, sizeof(send_buf));
	while (1)
	{
		memset(rec_buf, 0, sizeof(rec_buf));
		write_com(send_buf, 1);
		if (read_com(rec_buf, 1) == 1)
		{
            rec_len = 1;
            for (i = 0; i < 3; i++)
            {
    			Sleep(1);
    			rec_len += read_com(rec_buf + rec_len, sizeof(rec_buf) - rec_len);
                //printf("i = %d, rec_len=%d\r\n", i, rec_len);
    			if (rec_len >= 2)
    			{
                    if (rec_buf[0] == DEVICE_TO_HOST && rec_buf[1] == DOWNLOAD_STEP_ONE)
                    {
                        if (rec_len >= STEP_ONE_REC_LEN)
                        {
                            cur_life_stages = rec_buf[4];
                            printf("stage: %d ", rec_buf[4]);
                            printf("SN: ");
                            for (j = 0; j < 16; j++)
                                printf("%02X ", rec_buf[5+j]);
                            printf("\r\n");
        					return 0;
                        }
                    }
                    //else
                    //    return -1;
    			}
            }
		}
	}
}

int download_step_two(void)
{
#define STEP_TWO_REC_LEN    (29)
	unsigned int rec_len = 0,i = 0, j = 0;
	unsigned char send_buf[11];
	unsigned char rec_buf[200];
	
	memset(send_buf, 0x7C, sizeof(send_buf));
	clear_com();
	//while (1)
	{
		write_com(send_buf, 10);
		Sleep(2);
		memset(rec_buf, 0, sizeof(rec_buf));
        for (i = 0; i < 3; i++)
        {
    		rec_len += read_com(rec_buf + rec_len, sizeof(rec_buf) - rec_len);
    		if (rec_len >= 2)
    		{
    			if (rec_buf[0] == DEVICE_TO_HOST && rec_buf[1] == DOWNLOAD_STEP_TWO)
    			{
                    if (rec_len >= STEP_TWO_REC_LEN)
                    {
                        cur_life_stages = rec_buf[4];
                        printf("stage: %d ", rec_buf[4]);
                        printf("SN: ");
                        for (j = 0; j < 16; j++)
                            printf("%02X ", rec_buf[5+j]);
                        printf("\r\n");
                        return 0;
                    }
                }
    			else
    				return -1;
    		}
        }
		Sleep(1);

	}
}

int download_step_three(void)
{
#define STEP_THR_REC_LEN    (10)
    FILE *fp;            /*文件指针*/ 
	unsigned int rec_len = 0, send_len, i,j,n_len= 0,e_len = 0;
	unsigned char send_tmp[2050]={0x00,0x00};
	unsigned char send_buf[1024]={0};
    unsigned char pub_key_n[1024]={0};
    unsigned char pub_key_e[10]={0};
	unsigned char rec_buf[100];

    send_tmp[0] = cur_life_stages;
	send_len = download_packet(DOWNLOAD_STEP_THR, 2, send_tmp, send_buf);

    /* 注入密钥、SN等，升级成产品阶段。只有在调试阶段才有这一步  Modified by WLB at 2017.7.31 */
#if 0
    if (cur_life_stages = DEBUG_STAGES)
    {
        fp = fopen("RSA_key_test.rsa", "r");
        if(fp == NULL)
        {
            printf("RSA_key_test Error Opening File\r\n");
            return -1;
        }
        
        while(fgets(send_tmp, 2048, fp) != NULL) 
        {
            rec_len = strlen(send_tmp);
            //send_tmp[rec_len-1] = '\0';  /*去掉换行符*/
            //printf("%s %d \n", send_tmp, rec_len - 1);
            if (memcmp(send_tmp, "pub_key n:", 10) == 0)
            {
                for (i = 0; i < rec_len;)
                {
                    if (send_tmp[i+10] == '\r' || send_tmp[i+10] == '\n')
                        break;
                    for (j = 0; send_tmp[i+10+j] == ' '; j++);
                    sscanf(&send_tmp[i+10+j], "%02x", &pub_key_n[n_len++]);
                    //printf("pub_key_n[%d]=%02x", n_len - 1, pub_key_n[n_len - 1]);
                    //pub_key_n[n_len++] = atoi(send_buf[i+10+j]);//*/
                    i += j+2;
                }
            }
            if (memcmp(send_tmp, "pub_key e:", 10) == 0)
            {
                for (i = 0; i < rec_len;)
                {
                    if (send_tmp[i+10] == '\r' || send_tmp[i+10] == '\n')
                        break;
                    for (j = 0; send_tmp[i+10+j] == ' '; j++);
					sscanf(&send_tmp[i+10+j], "%02x", &pub_key_e[e_len++]);
                    //pub_key_e[e_len++] = atoi(send_buf[i+10+j]);
                    i += j+2;
                }
            }
        }

        //printf("n_len = %d, e_len=%d\r\n", n_len, e_len);
        memset(send_tmp, 0, sizeof(send_tmp));
        memset(send_buf, 0, sizeof(send_buf));
        /* E-RSA-PKEY(264B) pub_key_n = 256 Modified by WLB at 2017.7.31 */
        memcpy(&pub_key_n[256 + 8 - e_len], pub_key_e, e_len);
        i = 0;
        send_tmp[i++] = 0x03;
        send_tmp[i++] = 0x55;
        /* SN   Modified by WLB at 2017.7.31 */
        memcpy(&send_tmp[i], "\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38", 16);
        i += 16;
        /* E-Time(2B) Modified by WLB at 2017.7.31 */
        send_tmp[i++] = 0x32;
        send_tmp[i++] = 0x00;
        /* E -Scratch：表示flash加扰配置 已经作废，固定0x4000 Modified by WLB at 2017.7.31 */
        send_tmp[i++] = 0x00;
        send_tmp[i++] = 0x40;
        /* E –Reg-Num： 固定为0，保留。Modified by WLB at 2017.7.31 */
        send_tmp[i++] = 0x00;
        send_tmp[i++] = 0x00;
        /* E –Info：表示阶段参数的信息（RSA密钥号）Modified by WLB at 2017.7.31 */
        send_tmp[i++] = 0x01;
        send_tmp[i++] = 0x00;
        memcpy(&send_tmp[i], pub_key_n, 264);
        i += 264;
        send_len = download_packet(DOWNLOAD_STEP_THR, i, send_tmp, send_buf);
        //write_com(send_buf, send_len);
        fclose(fp);
        //while(1);
    }
#endif
	clear_com();
	//while (1)
	{
		write_com(send_buf, send_len);
		Sleep(2);
		memset(rec_buf, 0, sizeof(rec_buf));
        for (i = 0; i < 3; i++)
        {
    		rec_len += read_com(rec_buf + rec_len, sizeof(rec_buf) - rec_len);
    		if (rec_len >= 2)
    		{
    			if (rec_buf[0] == DEVICE_TO_HOST && (rec_buf[1] == STEP_ACK || rec_buf[1] == DOWNLOAD_STEP_THR))
    				return 0;
    			else// if (rec_buf[1] == STEP_NCK)
    			{
                    for (i = 0; i < rec_len; i++)
                        printf("%02x ", rec_buf[i]);
    				return -1;
    			}
    		}
            Sleep(2);
        }
	}
}

int download_step_four(void)
{
#define DATA_LEN            (297)
#define STEP_FOU_REC_LEN    (10)

    int handle;
	unsigned int rec_len = 0, send_len = 0, nbyte = 0, have_read=0,i;
    unsigned int fvaild_addr, crc32_value;
	unsigned char send_tmp[300]={0};
	unsigned char send_buf[400]={0};
	unsigned char rec_buf[100];
    unsigned char buf[1200]={0};
    unsigned char hash[70]={0};
    struct stat st ;
    SHA256Context sc;

    memset(hash, 0, sizeof(hash));
    memset(send_tmp, 0, sizeof(send_tmp));
	memset(send_buf, 0, sizeof(send_buf));
	memset(rec_buf, 0, sizeof(rec_buf));

    printf("stages %d\r\n", cur_life_stages);
    if (cur_life_stages == DEBUG_STAGES)
    {
        SHA256Init(&sc);
        stat( "project.bin", &st );
        //printf(" file size = %d\n", st.st_size);

    	handle = open("project.bin", O_RDONLY|O_BINARY, S_IREAD);
        if(handle == -1)
        {
            printf("ErrorOpeningFile\r\n");
            return -1;
        }
        else
        {
            while (have_read < st.st_size)
            {
                nbyte = read(handle, buf, (st.st_size - have_read) > 1024 ? 1024 : (st.st_size - have_read));
                SHA256Update (&sc, buf, nbyte);
                have_read += nbyte;
            }
            SHA256Final (&sc, hash);
            //for (i = 0; i < 64; i++)
            //    printf("%02x ", hash[i]);

    		close(handle);
        }

        send_tmp[send_len++] = 0x00;            // Refresh：表示本次下载是否刷新Flash扰码（未使能扰码时，此区域无效)
        memset(&send_tmp[send_len], 0xFF, 32);  //Erase：表示要擦除的flash区域（1Bit代表1个Page（4KB） 。 0b1： 擦除，0b0： 不擦除
        send_len += 32;
        memset(&send_tmp[send_len], 0x00, 4);   // F-Protect：表示启动时只读锁定的区域（1Bit代表8个Page，每Page 4KB）。0b1：锁定， 0b0：不锁定
        send_len += 4;
        fvaild_addr = send_len;
        memcpy(&send_tmp[send_len], "\xAA\xAA\x55\x55", 4); // F-Valid：表示文件头结构有效
        send_len += 4;

        /* F-Sign Modified by WLB at 2017.7.27 */
        memset(&send_tmp[send_len], 0x00, 4);   // sg_Res：填0，预留给RSA签名使用
        send_len += 4;
        send_tmp[send_len++] = (unsigned char)(START_ADDR);      // sg-Start：表示文件的起始位置
        send_tmp[send_len++] = (unsigned char)(START_ADDR >> 8);
        send_tmp[send_len++] = (unsigned char)(START_ADDR >> 16);
        send_tmp[send_len++] = (unsigned char)(START_ADDR >> 24);
        send_tmp[send_len++] = (unsigned char)(st.st_size);     // sg-Len：表示文件长度
        send_tmp[send_len++] = (unsigned char)(st.st_size >> 8);
        send_tmp[send_len++] = (unsigned char)(st.st_size >> 16);
        send_tmp[send_len++] = (unsigned char)(st.st_size >> 24);
        send_tmp[send_len++] = 0xFF;            // sg-Ver：表示程序文件版本
        send_tmp[send_len++] = 0xFF;
        send_tmp[send_len++] = 0x03;            // sg-Opt：表示使用的HASH选项， 3： SHA256； 5： SHA512；其他值：错误。
        send_tmp[send_len++] = 0x00;
        memcpy(&send_tmp[send_len], hash, 64);  // sg-Hash：表示固件文件的HASH校验值。
        send_len += 64;
        crc32_value = calculate_crc32(&send_tmp[fvaild_addr], send_len - fvaild_addr);
        send_tmp[send_len++] = (unsigned char)(crc32_value);               // sg-Crc32：表示F-Valid加上F-sign结构的CRC32校验值。
        send_tmp[send_len++] = (unsigned char)(crc32_value >> 8);
        send_tmp[send_len++] = (unsigned char)(crc32_value >> 16);
        send_tmp[send_len++] = (unsigned char)(crc32_value >> 24);
    }
    else if (cur_life_stages == PRODUCT_STAGES)
    {
        handle = open("Project.hex.sig", O_RDONLY|O_BINARY, S_IREAD);
        if(handle == -1)
        {
            printf("Project.hex.sig ErrorOpeningFile\r\n");
            return -1;
        }
        send_tmp[0] = 0x01;
        nbyte = read(handle, buf, 16);  // 16字节头部信息
        nbyte = read(handle, &send_tmp[1], 296);
        close(handle);
    }
    send_len = download_packet(DOWNLOAD_STEP_FOR, DATA_LEN, send_tmp, send_buf);
	clear_com();
	//while (1)
	{
		write_com(send_buf, send_len);
		Sleep(10);
		memset(rec_buf, 0, sizeof(rec_buf));
        for (i = 0; i < 3; i++)
        {
    		rec_len += read_com(rec_buf + rec_len, sizeof(rec_buf) - rec_len);
    		if (rec_len >= 2)
    		{
    			if (rec_buf[0] == DEVICE_TO_HOST && (rec_buf[1] == STEP_ACK || rec_buf[1] == DOWNLOAD_STEP_FOR))
    		    {
                    printf("正在擦除...\r\n");
                    return 0;
    			}
    			else// if (rec_buf[1] == STEP_NCK)
    				return -1;
    		}
            Sleep(5);
        }
	}

    return -1;
}

int download_step_five(void)
{
    int handle;
    unsigned int write_add = START_ADDR, unlock;
    unsigned int have_write_len = 0;
    unsigned int len = 0, read_len, send_len, rec_len,i;
    unsigned char *p;
	unsigned char send_tmp[1200]={0};
	unsigned char send_buf[1200]={0};
    struct stat st ;

    memset(send_tmp, 0, sizeof(send_tmp));
	memset(send_buf, 0, sizeof(send_buf));

    printf("正在下载固件...\r\n");

    if (cur_life_stages == DEBUG_STAGES)
    {
        stat( "project.bin", &st );
        printf("file size = %d\r\n", st.st_size);
        handle = open("project.bin", O_RDONLY|O_BINARY, S_IREAD);
    }
    else
    {
        stat("Project.hex.sig", &st );
        printf("file size = %d\r\n", st.st_size);
        st.st_size -= 1039;
        handle = open("Project.hex.sig", O_RDONLY|O_BINARY, S_IREAD);
        if(handle != -1)
            read(handle, send_tmp, 1039);
        memset(send_tmp, 0, sizeof(send_tmp));
    }
    if(handle == -1)
    {
        printf("ErrorOpeningFile\n");
        return -1;
    }

    while (have_write_len < st.st_size)
    {
        /* F-Unlock：表示写入此文件数据块时， Flash对应的只读寄存器的值（操作
            flash前需解除只读保护）。 0b0：只读， 0b1：读写   (4B)
            Modified by WLB at 2017.7.28 */
        if (START_ADDR < FLASH_BASE_ADDR ||
			START_ADDR > FLASH_BASE_ADDR + FLASH_SIZE)
		{
		    unlock = 0;	
		}
		else
		{
		    unlock = 1 << ((write_add) / FLASH_PROTECT_UNIT);
		}
        send_tmp[len++] = unlock;
        send_tmp[len++] = unlock>>8;
        send_tmp[len++] = unlock>>16;
        send_tmp[len++] = unlock>>24;
        send_tmp[len++] = (unsigned char)(write_add);
        send_tmp[len++] = (unsigned char)(write_add >> 8);
        send_tmp[len++] = (unsigned char)(write_add >> 16);
        send_tmp[len++] = (unsigned char)(write_add >> 24);
        read_len = (st.st_size - have_write_len) > 1024 ? 1024 : (st.st_size - have_write_len);
        read(handle, &send_tmp[len], read_len);
        len += read_len;
        send_len = download_packet(DOWNLOAD_STEP_FIV, len, send_tmp, send_buf);
        clear_com();
        //while (1)
        {
            memset(send_tmp, 0, sizeof(send_tmp));
            write_com(send_buf, send_len);
            rec_len = 0;
            for (i = 0; i < 3; i++)
            {
                rec_len += read_com(send_tmp + rec_len, sizeof(send_tmp) - rec_len);
                //printf("len=%d(%02x %02x)\r\n", rec_len, send_tmp[0], send_tmp[1]);
                if (rec_len >= 2)
                {
                    if (send_tmp[0] == DEVICE_TO_HOST && (send_tmp[1] == STEP_ACK || send_tmp[1] == DOWNLOAD_STEP_FIV))
                        break;
                    else
                    {
                        if (i < 3 && send_tmp[0] != DEVICE_TO_HOST)
                        {
                            if ((p = memchr(send_tmp, DEVICE_TO_HOST, rec_len)) == NULL)
                                continue;
                            else
                            {
                                if (p[1] == STEP_ACK || p[1] == DOWNLOAD_STEP_FIV)
                                    break;
                            }
                        }
                        Sleep(1000);
                        return -1;
                    }
                }
                Sleep(10);
            }
        }
        //Sleep(5);
        have_write_len += read_len;
        write_add += read_len;
        len = 0;
    }

    close(handle);
    return 0;
}

