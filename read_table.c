/*
 *  Read calibration table from file
 */

#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "decomment.h"


TRANS_TAB read_table(char *name_table) {
  FILE *fd_in;
  int c;
  double x;
  TRANS_TAB T;
  int i=0, j=0;

  for (i=0; i<MAX_C; i++) {  /* Clear note and table */
    if (i%2 == 0) 
      T.note[i%2][0] = '\0';
    for (j=0; j<MAX_R; j++)
      T.M[i][j]=0;
  }

  fd_in = decomment(name_table);  /* Decomment input plain file */
  
  i=0; j=0;
  while ((c=getc(fd_in)) != EOF) {
    ungetc(c,fd_in);
    if (i>=MAX_R) {
      fprintf(stderr,"Number of rows in table '%s' exceeded! (%d)\n",name_table,i);
      exit(EXIT_FAILURE);
    }
//  printf("c=%X\n",c);
    while (c != '\n' && c != EOF) {
      if (j>=MAX_C) {
        fprintf(stderr,"Number columns in table '%s' columns exceeded! (%d)\n"\
          ,name_table,j);
        exit(EXIT_FAILURE);
      }
      if (c == ';') {  /* After symbol ';' read note from file */
        getc(fd_in);  /* Throw away ';' */
        int k=0;
        while ((c=getc(fd_in)) != '\n' && c != EOF && k<MAX_CHAR-1) {
          T.note[i/2][k++] = c;
        }
        T.note[i/2][k] = '\0';
      }
      fscanf(fd_in,"%lf",&x);  /* Read number */
    T.M[i][j++] = x;
//      printf ("%lf ",x); 
      while ((c=getc(fd_in)) == ' ');  /* Skip spaces */
      if (c != '\n')
        ungetc(c,fd_in);
    }
//    putchar('\n');
    T.col[i] = j;
    i++;
    j = 0;
  }

/* Tests consistation of tables */
  if (i%2) {
    fprintf(stderr,"Number of rows in table '%s' must be even (is %d)!\n"\
        ,name_table,i);
    exit(EXIT_FAILURE);
  }

  T.tables = i/2;

  for (i=0; i<2*T.tables; i+=2) {
    if (T.col[i]!=T.col[i+1]) {
      fprintf(stderr,"Non equal number x and f(x) in table %d ! (%d != %d)\n"\
        ,i/2,T.col[i],T.col[i+1]);
      exit(EXIT_FAILURE);
    }
  }
    
  return T;
}
