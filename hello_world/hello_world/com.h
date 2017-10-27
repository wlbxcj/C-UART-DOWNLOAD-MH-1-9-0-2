#ifndef __COM_H__
#define __COM_H__

int open_com(void);
int read_com(unsigned char *buf, unsigned int read_len);
int write_com(unsigned char *buf, unsigned int write_len);
int close_com(void);
void clear_com(void);
#endif