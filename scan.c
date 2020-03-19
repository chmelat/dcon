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
 *  Scan adresniho prostoru od adresy 'adr' do  adresy 255
 *  V0.1/2015-07-30
 */
int adam_scan(int fd, unsigned char adr)
{
  int i, j;  /* Prubezny index */
  int length;
  char token[LTMAX];
  char buf[LTMAX];


/* Tisk hlavicky */
  printf("\nDevice address scan:\n");

/* Nekonecny merici cyklus */
  for (; adr<255; adr++) {
    printf("Address %02XH (%03d) ... ",adr,adr); fflush(stdout);

    sprintf(token,"$%02XM",adr);  /* Token pro ADAMa (Read module name) */
    send_token(fd,token);
    if (received_token(fd,token) < 0) { /* Odpoved ADAMa */
      putchar('\r');  /* Navrat voziku (CR), Adresa nekomunikuje */
      continue;
    }
    /* Vyhodnoceni odpovedi */
    length = strlen(token); /* Pocet znaku tokenu */
    if ( length > 3 ) {  /* Adresa komunikuje */
      i = 3; /* Preskoc '!AA' (pocatek odpovedi ADAMa) */
      j = 0;
      while (token[i] != '\0' && i < LTMAX-1) {
        buf[j++] = token[i++];
      }
      buf[j] = '\0'; /* Terminacni znak */
      printf("%s\n",buf);
    }
    else
      putchar('\r');  /* Navrat voziku (CR), Adresa nekomunikuje */
  }
  return 0;
}
