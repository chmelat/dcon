/*
 *  Function for read and setting channels
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
 *  Pri uspechu je navratova hodnota 0, pri nezdaru cteni je navratova hodnota -1
 *  v pripade chybne odpovedi je navratova hodnota -2.
 *
 */
int set_channels(int fd, unsigned char adr, unsigned int nc) 
{
  char token[LTMAX];
  char buf[8];
  char c_hi;
  char c_lo;
  
  n2bytes(nc, &c_hi, &c_lo); /* prevod 'nc' na c */

  sprintf(token,"$%02X5%c%c",adr,c_hi,c_lo);  /* Formulace tokenu - prikaz ADAMa */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)
    return -1;

  sprintf(buf,"!%02X",adr); /* Predpokladana odpoved ADAMa*/ 

  if (strncmp(buf,token,3) != 0)
    return -2;

  printf("set_channels: ");
  showbits(nc);

  return 0;
}


/*
 *  Stanoveni poctu a cisel ctenych kanalu ADAMa:
 *  $AA6  --> !AAab
 *  kde 'a' oznacuje 4 bity kanalu 4-7, 'b' kanaly 0-3
 *  Pri uspechu je navratova hodnota 0. Pri nezdaru cteni dat z ADAMa je navratova hodnota -1,
 *  v pripade chybne odpovedi z ADAMa je navratova hodnota -2.
 */
int ch_mon(int fd, unsigned char adr, int ch[8])
{
  int i;
  int nch=0;
  char token[LTMAX];
  char buf[8];
  unsigned int hi,lo;

/* Dotaz na ADAMa - jake jsou ctene kanaly? */  
  sprintf(token,"$%02X6",adr);  /* Formulace tokenu pro ADAMa */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Odpoved ADAMa */
    return -1;

  sprintf(buf,"!%02X",adr); /* Predpokladana odpoved ADAMa*/ 
  if (strncmp(buf,token,3) != 0) 
    return -2;

/* Prevod ascii znaku na uint */
  sprintf(buf,"%c",token[3]); 
  sscanf(buf,"%x",&hi);
  sprintf(buf,"%c",token[4]); 
  sscanf(buf,"%x",&lo);

/* Poradi a pocet ctenych kanalu... */  
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
  return nch;  /* Pocet otevrench kanalu */
}

/***  Lokalni fce  ***/

/*
 *  Prevod cisla 'nc' na 2 byte 4-bitoveho kodovani
 *  c[0] je vyznamnejsi byte (kanaly 4-7)
 *  c[1] je mene vyznamny byte (kanaly 0-3)
 */
static void n2bytes(unsigned int nc, char *c_hi, char *c_lo)
{
    unsigned int hi, lo;
    
    lo = (nc&15);    /* logicky soucin s '00001111' */
    hi = (nc>>4)&15; /* bitovy posun o 4 doprava a log. soucin */

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
static void showbits(unsigned int x)
{
  int i; 
  for(i=(sizeof(unsigned int)*8-1); i>=0; i--)
    (x&(1<<i))?putchar('1'):putchar('0');
  printf("\n");
}
