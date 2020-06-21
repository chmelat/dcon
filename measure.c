/*
 *  Continual read data from data acquising modules via protocol DCON
 *  V0/29.7.2015/TCh
 *  V1.0/2018-03-21 Pocet desetinnych mist v zavislosti na typu ADAMa 
 *  V1.1/2019-12-04 Add ICP_DCON number of modules to select resolution 
 *  V1.2/2020-02-06 Add transformation function - interpolation from tables
 *  V1.3/2020-06-02 Add resolution 'int res' to formal parameters
 *  V1.4/2020-06-21 New format of output filename
 */

#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>

#include "time.h"
#include "io_token.h"
#include "channels.h"
#include "typedef.h"
#include "interpolation.h"
#include "table_interval.h"
#include "measure.h"

/*
 * Lokalni int konstanty
 */
enum {
  LTMAX = 128,  /* Maximal length of token include char '\0' */
};

/*
 *  Declare local functions
 */
static int get_value(int fd, unsigned char adr, int nch, double val[8]);
static int device_name(int fd, unsigned char adr, char *name);
// static int resolution(int fd, unsigned char adr, char *name);


/*
 * Repeated measure, return value is 0 after time define in parameter w.
 * Otherwise -1 is returned.
 */
int conti_measure(int fd, unsigned char adr, double interval, int w, int itt, int z[8][2], int f, int res)
{
  int nch;  /* Number of measured channels */
  int ch[8];  /* Log. val. (0,1) select channels (0-7) */
  double val[8];  /* Read data */
  int i,j,k;
  time_t start, end, actual; /* Time [s] (from year 1970) */
  int z_fail;
  FILE *fp[2];  /* Pointer to output device (terminal, file) */
  char name[LTMAX]; /* Module name */

  if (f<0 || f>1) {
    fprintf(stderr, "conti_measure: Incorrect value f = %d!\n",f);
    exit(EXIT_FAILURE);
  }

  fp[0] = stdout;
/* Open output file, name according to time */
  if (f==1) {
    if ( (fp[1]=fopen(now(adr), "w")) == NULL ) {
      fprintf(stderr, "Can't open file '%s' for writing.\n",now(adr));
      exit(EXIT_FAILURE);
    }
  }

/* Select channels */
  nch = ch_mon(fd, adr, ch);
//  {
//    int i;
//    printf("nch = %d\n",nch);
//    for (i=0; i<8; i++) {
//      printf("ch[%d] = %d\n",i,ch[i]);
//    }
//  }
  if (nch < 0) {
    fprintf(stderr,"conti_measure: Error in channel select!\n");
    exit(EXIT_FAILURE);
  } 
#if 0  
  res = resolution(fd, adr, name); /* Device resolution */
  if (res < 0) {
    fprintf(stderr,"conti_measure: Error in resolution decise!\n");
    exit(EXIT_FAILURE);
  }
#endif

  /* Device name */
  device_name(fd, adr, name); 

  for (j=0; j<itt; j++) {
    z_fail = 1;
    for (i=0; i<nch; i++) {
      if (ch[i] == z[j][0]) {
        z_fail = 0;  /* Find channel */
        break;
      }
    }
    if (z_fail) {
      fprintf(stderr,"conti_measure: Channel %d is disable, not allowed use option -z %d,n!\n",z[j][0],z[j][0]);
      exit(EXIT_FAILURE);
    }
  }

/* Time control */
  time(&start);
  if (w==1) 
    end = start + 60; /* One min [s] */	
  else if (w==2) 
    end = start + 60*60; /* One hour [s] */
  else if (w==3) 
    end = start + 60*60*24; /* One day [s] */
  else if (w==4) 
    end = start + 60*60*24*7; /* One week [s] */
  else 
    end = 0;

/* Print head of device responeses */
  for (k=0; k<f+1; k++) {
    fprintf(fp[k],"# %s\n",dev_name);
    fprintf(fp[k],"# DATE TIME             ");
    for (i=0; i < nch; i++)
      fprintf(fp[k],"%-9s",sensors[ch[i]]);
    fprintf(fp[k],"\n#                       ");
    for (i=0; i < nch; i++)
      fprintf(fp[k],"Ch_%-6d",ch[i]);
    fprintf(fp[k],"\n");
    fprintf(fp[k],"# Device type: %s\n",name);
    fprintf(fp[k],"# Device address: %02XH\n",adr);
//    fprintf(fp[k],"# Number of print decimal digits: %d\n",res);
//    fprintf(fp[k],"# Continual measure, dt = %.1lf s\n" , interval);
    for (i=0; i<itt; i++)
      fprintf(fp[k],"# Transformation channel Ch_%d via table %d: %s %s\n",z[i][0],z[i][1],T.note[i],table_interval(z[i][1]));
    fflush(fp[k]); /* Flush buffer */
  }

/* Infinite measure cycle */
  for (;;) {
    if ( get_value(fd, adr, nch, val) == 0 ) { /* Read values from module */
/* Print output */
      for (k=0; k<f+1; k++) {
        fprintf(fp[k],"%s  ", now(0));  /* Actual date and time */
        for (i=0; i < nch; i++) {
          for (j=0; j<itt; j++) {
            if (ch[i] == z[j][0] && k == 0) {  /* Translate select channels by interp. from  table */
              val[i] = interpolation(val[i],z[j][1]);
              break;
            }
          }
//          (val[i] != 888888 && val[i] != -9999.9) ? fprintf(fp[k],"%-8.*f ",res,val[i]) : fprintf(fp[k],"NaN      "); /* Print 'res' frac. decimals */
          fprintf(fp[k],"%-8.*f ",res,val[i]); /* Print 'res' frac. decimals */
        }
        fprintf(fp[k],"\n");
        fflush(fp[k]); /* Flush buffer */
      }
    }
    time(&actual);
    if (w && actual > end) {
      return 0;	    /* End of measure after time accord parameter w */
    }
/* Wait [us] (input: interval [s]) */
    usleep((useconds_t)(interval*1000000));
  }
  return -1;
}


/*
 * Read values from opened channels (total 'nch').
 * In successful is return value 0, otherwaise -1.
 */
static int get_value(int fd, unsigned char adr, int nch, double val[8])
{
  int i, j, k;
  int length;
  char token[LTMAX];
//  char *stest = ">+0001.2-1234.5-1234.5-1234.5-1234.5-1234.5-1234.5-1234.5"; /* debug... */
  char buf[16]; /* Pole pro zapis cteneho cisla */
  double d;
  static char *msg = "get_value";  /* Message */

//  printf("token: #%02X\n",adr);
  snprintf(token,LTMAX,"#%02X",adr);  /* Token formulation */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Module response */
    return -1;

//  printf("%s\n",token);

  length = strlen(token);
  i = k = 0;
  while (i < length && k < nch) {
    while (!(token[i] == '+' || token[i] == '-')) {  /* Skip characters until '+' or '-' */
      i++;
    }
    buf[0] = token[i++];  /* Write sign in buffer begin */
    j = 1;
    while (isdigit(token[i]) || token[i]=='.')
      buf[j++] = token[i++];
    buf[j] = '\0';  /* Buffer termination */
    if ( sscanf(buf, "%lf", &d) == EOF ) {  /* Read number from buffer */
//      perror(msg);
      fprintf (stderr, "%s: %s\n", msg, strerror(errno));
      exit(EXIT_FAILURE);
    }
    val[k++] = d;
  }
    return 0;
}


#if 0
/*
 * In success function returned number of decimal digits rought output,
 * otherwise returned -1.
 * Sampling frequency module 7019R (ICP_CON) is about 15 Hz
 */
static int resolution(int fd, unsigned char adr, char *name)
{
  int res = 1; /* Default value */
  static char token[LTMAX];
  static char *buf[] = {"4015","4013","7015","7013","7019R","4018P",0}; /* Devices (ADAM or ICP_CON) with high resolution */
  char **p = buf;

  snprintf(token,LTMAX,"$%02XM",adr);  /* Question for name */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Response */
    return -1;

//  printf("# Module type: %s\n",token+3);
  strncpy(name,token+3,LTMAX-1);

  while (*p) {
    if (strncmp(*p++,token+3,4) == 0) {
      res = 2; /* High resolution (2 digits) */
      break;
    }
  }
  return res;
}
#endif

/*
 *  Device name
 */
static int device_name(int fd, unsigned char adr, char *name)
{
  static char token[LTMAX];
  snprintf(token,LTMAX,"$%02XM",adr);  /* Question for name */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Response */
    return -1;
  strncpy(name,token+3,LTMAX-1);
  return 0;
}

