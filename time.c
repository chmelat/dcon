/*
 *  Fce pro ziskani casu v std. podobe
 *  V1.0/2012-12-05
 *  V1.1/2020-02-10 Add parameter int f
 */
#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h>  /* Standard lib */
#include <string.h>   /* Manipulating arrays of characters */
//#include <unistd.h>  /* UNIX standard function definitions */
#include <time.h>  /* Time function */
#include <sys/time.h>  /* Time function */

#define PATH "./"  /* Path for file*/

/*
 *  Actual date and time
 *  if (f == 0) then return time, if (f==1) then return name of file 
 */
char *now(int f)
{
  struct tm  *ts;  /* Time */
  static char buf[64];  /* Textova promenna pro zapis casu */
  static char buf_file[64];  /* Filename according to time */
  struct timeval now;  /* Argument of gettimeofday */

/* Nacteni casu a jeho zapis do promenne buf v pozadovanem formatu */
  if (gettimeofday(&now, NULL) == -1) {; /* seconds from year 1970 a ppm sec*/
    fprintf(stderr, "Error in call gettimeofday(&now,NULL)\n");
    perror("gettimeofday");
    exit (EXIT_FAILURE);
  }
  ts = localtime(&now.tv_sec);  /* Transformation seconds to time structure */

  if (f==0) {
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ts);  /* Conversion time structure to desired form */
    sprintf(buf, "%s.%02u", buf, (unsigned)(now.tv_usec/1E4));  /* Add decimal fraction */
   /* printf("%06u\n", now.tv_usec); */  /*ladici*/
  }
  else if (f==1) {
    strftime(buf, sizeof(buf), "%y%m%d-%H_%M", ts); /*Konv. cas. str. na cit. format */
    sprintf(buf_file,"%s/M_%s.dat",PATH,buf);  /* Pripojeni cesty a koncovky */
    strcpy(buf,buf_file);
  }
  else {
    fprintf(stderr,"now: Unknown parameter %d\n" ,f);
    exit (EXIT_FAILURE);
  }
  return buf;
}
