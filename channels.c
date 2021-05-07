/*
 *  Function for read and setting module channels
 */

#include <stdio.h>  /* Standard input/output definitions */
#include <string.h> /* */

#include "io_token.h" /* Prijem a odeslani tokenu */

/*
 * Local constants
 */
enum {
  LTMAX = 128,  /* Maximalni delka tokenu vcetne znaku '\0' */
};

/*
 *  Declare functions
 */
static void n2bytes(unsigned int nc, char *c_hi, char *c_lo);
static void showbits(unsigned int x);  /* Vypis bitu cisla na terminal */

/*
 *  Set channel
 *  In success is return value 0, after failure is return val. -1
 *  At wrong answer is return value -2.
 *
 */
int set_channels(int fd, unsigned char adr, unsigned int nc) 
{
  char token[LTMAX];
  char buf[8];
  char c_hi;
  char c_lo;
  
  n2bytes(nc, &c_hi, &c_lo); /* Transform number 'nc' to char 'c' */

  snprintf(token,LTMAX,"$%02X5%c%c",adr,c_hi,c_lo);  /* Token construct */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)
    return -1;

  snprintf(buf,8,"!%02X",adr); /* Assumed response */ 

  if (strncmp(buf,token,3) != 0)
    return -2;

  printf("set_channels: ");
  showbits(nc);
  putchar('\n');

  return 0;
}


/*
 *  Numbers and position of modules channels:
 *  $AA6  --> !AAab
 *  where 'a' is 4 bits of channels 4-7 and 'b' channels 0-3
 *  In success is return value 0, after failure is return val. -1
 *  At wrong answer is return value -2.
 */
int ch_mon(int fd, unsigned char adr, int ch[8])
{
  int i;
  int nch=0;
  char token[LTMAX];
  char buf[8];
  unsigned int hi,lo;

/* Question - what are read channels? */  
  snprintf(token,LTMAX,"$%02X6",adr);  /* Token construct */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Module response */
    return -1;

  snprintf(buf,8,"!%02X",adr); /* Assumed response */ 
  if (strncmp(buf,token,3) != 0) 
    return -2;

/* Transformation ASCII char to uint */
  snprintf(buf,8,"%c",token[3]); 
  sscanf(buf,"%x",&hi);
  snprintf(buf,8,"%c",token[4]); 
  sscanf(buf,"%x",&lo);

/* Position and number read channels */  
  for (i=0; i<4; i++) {
    if ( lo & (1<<i) ) {
      ch[nch++] = i; 
    }
  }

  for (i=0; i<4; i++) {
    if ( hi & (1<<i) ) {
      ch[nch++] = i+4;
    }
  }
  return nch;  /* Number of opened channels */
}

/*
 * Lokalni functions 
 */

/*
 *  Translation number 'nc' to 2 byte 4-bit encoding
 *  c[0] is high byte (channels 4-7)
 *  c[1] is lowes byte (channels 0-3)
 */
static void n2bytes(unsigned int nc, char *c_hi, char *c_lo)
{
    unsigned int hi, lo;
    
    lo = (nc&15);    /* Logical product with '00001111' */
    hi = (nc>>4)&15; /* Bits shift for 4 right and log. product */

    if (hi <= 9)
      *c_hi = (char)hi + '0';
    else
      *c_hi = (char)hi + ('A' - 10);  

    if (lo <= 9)
      *c_lo = (char)lo + '0';
    else
      *c_lo = (char)lo +  ('A' - 10);  
}


/*
 *  Write bits of number on terminal
 */

void showbits(unsigned int x)
{
  static unsigned int i = 0;

    if (++i < sizeof(char)*8) {
    showbits(x >> 1); /* Recursion, bit shift 1 right (null from left) */
  }
  putc((x & 1) ? '1' : '0', stdout);
  i = 0;
}
