/*
 *  Odeslani a prijmuti tokenu na zarizeni oznacene popisovacem 'fd'
 *  protokolem DCON ('\r' je terminacni znak)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  /* UNIX standard function definitions */
//#include <errno.h>

#include "io_token.h"

#define LTMAX 128  /* Maximal length of token (include '\0') */
#define CR '\r'

/*
 *  Send token to serial device (write)
 */
int send_token (int fd, char *p_token)
{
  size_t length;  /* Length of data */

  length = strlen(p_token); /* delka tokenu */
//  printf("Send %zu char.\n",length);

  if (length > LTMAX-2) {
    fprintf(stderr, "send_token: Token is too long.\n");
    exit (EXIT_FAILURE);
  }

  p_token[length] = CR;  /* Terminacni znak */
  p_token[length+1] = '\0';  /* End of token */

  if (write (fd, p_token, length+1) < 0) {
    perror("send_token");
    exit (EXIT_FAILURE);
  }
  else
    return 0;  /* Succesful send token */
}


/*
 *  Recieve token from serial device (read)
 */
int received_token (int fd, char *p_token)
{
  char c = '\0';
  ssize_t res;  /* pocet ctenych dat */
  int i = 0;
/* Nacteni dat po 1 byt */
  while (c != CR) {
//    printf("ctu znak\n");
    res = read(fd, &c, 1); /* Cte data po 1 byte */
//    putchar(c);
    if (res == 1)
      p_token[i++] = c;
    else if (res < 0) {
      fprintf(stderr, "received_token: Read problem, res = %zd\n",res);
      exit (EXIT_FAILURE);
    }
    else if (res == 0) {
//      printf("received_token: No response (End-of-file)\n");
      i = -1;
      break;
    }
    if (i >= LTMAX-1) {
      fprintf(stderr, "received_token: Token is too length.\n");
      exit (EXIT_FAILURE);
    }
  }
  p_token[i] = '\0';
//  printf("Received %zu char.\n",i);
  return i;  /* Pocet prijatych znaku */
}
