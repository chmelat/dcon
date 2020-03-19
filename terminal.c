/*
 *  Simple text terminal for communication with device
 *  V0.1/2015-07-17
 */
#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <unistd.h>  /* UNIX standard function definitions */
//#include <errno.h>

#include "io_token.h"
#include "terminal_help.h"

#define LTMAX 128  /* Maximalni delka tokenu (vcetne '\0') */

int terminal(int fd)
{
  char token[LTMAX];
  int c; /* character */
  int i = 0;

  printf("Starting terminal ...\ntype q! for exit, ? for help\n");

  for (;;) {
/* Read token from keyboard */
    i=0;
    printf(">"); fflush(stdout);
    while ((c=getchar()) != '\n') {
      token[i++] = c;
      if (i >= LTMAX-1) {
        fprintf(stderr, "terminal: Token is too length, crop.\n");
	while ((c=getchar()) != '\n')  /* Ignoruj dalsi znaky */
          ;
	ungetc(c,stdin); /* navrat '\n' do fronty */
      }
    }

    token[i] = '\0'; /* Ukonceni tokenu */

    if (strcmp(token,"q!\0") == 0) {  /* q! je std. ukonceni terminalu */
      printf("Terminal exit ...\n"); fflush(stdout);
      return 0;
    }
    else if (strcmp(token,"?\0") == 0) { /* ? for help */
      terminal_help();
      continue;
    }

    printf("In: %s\n",token);

/* Send token to serial device */
    send_token (fd, token);

/* Read token from device */
    received_token (fd, token);
    printf("Out: %s\n", token);
  }
}
