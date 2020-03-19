/*
 *  Data structure of Calibration Table
 */

#define MAX_C 12 /* Max column of table */
#define MAX_R 16  /* 2*MAX_TABLES */
#define MAX_CHAR 72 /* Max char. of note */

/*
 * Transformation tables x -> f(x).
 * Even rows of M[][] are x value, odd rows are f(x) value,
 * col[] is vector numbers of columns in rows,
 * rows is number of rows table M 
 */
typedef struct {
  double M[MAX_R][MAX_C];
  int col[MAX_R];
  int tables; /* Number of tables */
  char note[MAX_R/2][MAX_CHAR];
} TRANS_TAB;
