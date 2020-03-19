/*
 *  Send message about intervals of calibration table
 */

#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "table_interval.h"

char *table_interval(int t) {
  static char msg[256];

  if (t < 0 || t >= T.tables) {
    fprintf(stderr,"interpolation: Table must be from interval 0..%d (%d)!\n",\
            T.tables-1,t);
    exit(EXIT_FAILURE);
  }

  sprintf(msg,"(%.2f..%.2f) -> (%.2f..%.2f)",\
          T.M[2*t][0],T.M[2*t][T.col[2*t]-1],\
          T.M[2*t+1][0],T.M[2*t+1][T.col[2*t]-1]);
  return msg;
}
