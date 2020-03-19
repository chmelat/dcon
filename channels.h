/*
 *  io_token.h
 *  V1/17.7.2015/TCh
 *
 */

/*
 *  deklarace globalnich fci
 */
extern int set_channels(int fd, unsigned char adr, unsigned int nc); 
extern int ch_mon(int fd, unsigned char adr, int ch[8]);
