/*
 *  Time print funftion
 *  V1.0/2012-12-05
 *  V1.1/2020-02-10 Add parameter int f
 */
#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h>  /* Standard lib */
//#include <string.h>   /* Manipulating arrays of characters */
//#include <unistd.h>  /* UNIX standard function definitions */
#include <time.h>  /* Time function */
#include <sys/time.h>  /* Time function */

#define PATH "."  /* Path for file*/
#define DBUF 64

/*
 *  Actual date and time
 *  if (f == 0) then return time, if (f==1) then return filename 
 */
char *now(int f)
{
  struct tm  *ts;  /* Time */
  static char buf[DBUF];
  static char buf_tmp[DBUF];
  struct timeval now;  /* Argument of gettimeofday */

/* Get time */
  if (gettimeofday(&now, NULL) == -1) {; /* seconds from year 1970 a ppm sec*/
    fprintf(stderr, "Error in call gettimeofday(&now,NULL)\n");
    perror("gettimeofday");
    exit (EXIT_FAILURE);
  }

  ts = localtime(&now.tv_sec);  /* Transformation seconds to time structure */

  if (f==0) {
    /* Conversion time structure to desired form */
    strftime(buf_tmp,sizeof(buf),"%Y-%m-%d %H:%M:%S",ts);
    snprintf(buf,DBUF,"%s.%02u",buf_tmp,(unsigned)(now.tv_usec/1E4));  /* Add fraction */
   /* printf("%06u\n", now.tv_usec); */  /*ladici*/
  }
  else if (f==1) {  //
    /* Conversion time structure to desired form */
    strftime(buf_tmp,sizeof(buf),"%y%m%d-%H_%M",ts);
    snprintf(buf,DBUF,"%s/M_%s.dat",PATH,buf_tmp);  /* Add path and suffix */
//    strcpy(buf,buf_file);
  }
  else {
    fprintf(stderr,"now: Unknown parameter %d\n",f);
    exit (EXIT_FAILURE);
  }
  return buf;
}
