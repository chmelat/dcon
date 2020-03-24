/*
 *  Scan device address space 
 *  V0.1/2015-07-30
 */

#include <stdio.h>  /* Standard input/output definitions */
//#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <string.h> /* */
//#include <errno.h>  /*  */

#include "io_token.h"

/*
 *  Functional type and local functions
 */

/*
 * Local constants
 */
enum {
  LTMAX = 128  /* Maximalni delka tokenu vcetne znaku '\0' */
};


/*
 *  Scan adress from 'adr' to 255
 *  V0.1/2015-07-30
 */
int adam_scan(int fd, unsigned char adr)
{
  int i, j;  /* Prubezny index */
  int length;
  char token[LTMAX];
  char buf[LTMAX];


  printf("\nDevice address scan:\n");

/* Scan cycle */
  for (; adr<255; adr++) {
    printf("Address %02XH (%03d) ... ",adr,adr); fflush(stdout);

    snprintf(token,LTMAX,"$%02XM",adr);  /* Token: Read module name */
    send_token(fd,token);
    if (received_token(fd,token) < 0) { /* Response */
      putchar('\r');  /* Adress not response, send CR */
      continue;
    }
    /* Response o.k. */
    length = strlen(token);
    if ( length > 3 ) {
      i = 3; /* Skip '!AA' (begin of response) */
      j = 0;
      while (token[i] != '\0' && i < LTMAX-1) {
        buf[j++] = token[i++];
      }
      buf[j] = '\0'; /* Terminal char */
      printf("%s\n",buf);
    }
    else
      putchar('\r');  /* Some error, send CR */
  }
  return 0;
}
