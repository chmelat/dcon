/*
 *  Decomment, remove all characters until '\n' after char '#'
 *  V4/2016-12-24/TCh
 */

#include <stdio.h>
#include <stdlib.h>  
#include <errno.h>
#include <string.h>

FILE *decomment(const char *name)
{
  FILE *fr, *fw_tmp;  /* File descriptors */
  int c;  /* Char */
  int p = 0;  /* Position in line  */

/* Open input file */  
  if ((fr = fopen(name, "r")) == NULL ) {   
    fprintf(stderr, "Failed to open file '%s' (%d: %s)\n",
            name, errno, strerror(errno));
    exit (EXIT_FAILURE);
  }

/* Create output (tmpfile) */  
  if ((fw_tmp = tmpfile()) == NULL) {
    fprintf(stderr, "Failed to open tmpfile. (%d: %s)\n",
            errno, strerror(errno));
    exit (EXIT_FAILURE);
  }

  while ((c=getc(fr)) != EOF) {
    if (c == '#')
      while ((c=getc(fr)) != '\n')  /* Skip all until \n */
        ;

    if (c == '\n' && p == 0)
      continue;

    if (c == '\n')
      p = 0;
    else
      p++;

    putc(c, fw_tmp);
  }

  if (fclose(fr) != 0) {
    fprintf(stderr, "Close failed. (%d: %s)\n",
            errno, strerror(errno));
    exit (EXIT_FAILURE);
  }

/* Set to begin file */
#if 0  
  if (fseek(fw_tmp, 0, SEEK_SET) != 0) {
    fprintf(stderr, "Set pointer to beginning failed.\n");
    exit (EXIT_FAILURE);
  }
#endif

  rewind(fw_tmp);
	 
 return fw_tmp;
}
