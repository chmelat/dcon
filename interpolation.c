/*
 *  Lagrange Interpolation
 */

#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "read_table.h"
#include "interpolation.h"


/*
 *  Lagrange polynom 
 *
 *               (x-a_1)..(x-a_{j-1})(x-a_{j+1})..(x-a_n)
 *  l_j(x) = ------------------------------------------------
 *           (a_j-a_1)..(a_j-a_{j-1})(a_j-a_{j+1})..(a_j-a_n)
 *
 */
static double lagrange(double x, int j, int t) {
  int i;
  double numerator=1, denominator=1; 
  for (i=0; i<T.col[2*t]; i++) {
  if ( i!=j ) {
      numerator *= (x - T.M[2*t][i]);
      denominator *= (T.M[2*t][j] - T.M[2*t][i]);
    }
  }

  if (denominator == 0) {
    fprintf(stderr,"lagrange: Zero division!\n");
    exit(EXIT_FAILURE);
  }

  return (numerator/denominator);
}

/* 
 *  Interpolation function 
 *  y(x) = Sum_{j=1}^n {l_j(x) f(a_j)}
 *  where l_j(x) is Lagrange polynom and f(a_j) is 'j' table point 
 *
 */
double interpolation(double x, int t) {
  int j;
  double y = 0;

  if (t < 0 || t>= T.tables) {
    fprintf(stderr,"interpolation: Table must be from interval 0..%d (%d)!\n",T.tables-1,t);
    exit(EXIT_FAILURE);
  }

  for (j=0; j<T.col[2*t]; j++) {
    y += lagrange(x,j,t)*T.M[2*t+1][j];
  }
  return y;
}
