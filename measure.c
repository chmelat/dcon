/*
 *  Kontinualni cteni dat z modulu ADAM (zkouseno na modulech 4018, 4018+, 4015)
 *  V0/29.7.2015/TCh
 *  V1.0/2018-03-21 Pocet desetinnych mist v zavislosti na typu ADAMa 
 *  V1.1/2019-12-04 Add ICP_DCON number of modules to select resolution 
 *  V1.2/2019-02-06 Add transformation function - interpolation from tables defined in 'interpolation.c'
 */

#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <string.h> /* */
#include <ctype.h>  /* Makra kontroly typu znaku */
#include <errno.h>  /*  */
#include <time.h>

#include "time.h"  /* Datum a cas */
#include "io_token.h"
#include "channels.h"
#include "typedef.h"
#include "interpolation.h"
#include "table_interval.h"

/*
 * Lokalni int konstanty
 */
enum {
  LTMAX = 128,  /* Maximalni delka tokenu vcetne znaku '\0' */
};

/*
 *  Funkcni prototypy lokalnich fci
 */
static int get_value(int fd, unsigned char adr, int nch, double val[8]);
static int resolution(int fd, unsigned char adr, char *name);


/* Opakovane  mereni,
 * RC=0 po uplynuti casu daneho parametrem 'w', jinak -1, f=0 for disable output to file, f=1 for enable output to file */
int conti_measure(int fd, unsigned char adr, double interval, int w, int itt, int z[8][2], int f)
{
  int nch;  /* Pocet merenych kanalu */
  int ch[8];  /* Log. hod. (0,1) vyb. kan. (0-7) */
  double val[8];  /* Merena data */
  int i,j,k;  /* Indexs */
  time_t start, end, actual; /* Casy [s] (od r. 1970) */
  int res = 1; /* Pocet vypisovanych des. mist */
  int z_fail;
  FILE *fp[2];  /* Pointer to output device (terminal, file) */
  char name[LTMAX]; /* Module name */

  if (f<0 || f>1) {
    fprintf(stderr, "conti_measure: Incorrect value f = %d!\n",f);
    exit(EXIT_FAILURE);
  }

  fp[0] = stdout;
/* Otevreni vystupniho souboru pojmenovaneho podle casu */
  if (f==1) {
    if ( (fp[1]=fopen(now(1), "w")) == NULL ) {
      fprintf(stderr, "Can't open file '%s' for writing.\n",now(2));
      exit(EXIT_FAILURE);
    }
  }

/* Vyber kanalu a jejich pocet pro vypis */
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
  res = resolution(fd, adr, name); /* Device resolution */
  if (res < 0) {
    fprintf(stderr,"conti_measure: Error in resolution decise!\n");
    exit(EXIT_FAILURE);
  }

  for (j=0; j<itt; j++) {
    z_fail = 1;
    for (i=0; i<nch; i++) {
      if (ch[i] == z[j][0]) {
        z_fail = 0;  /* Nalezen odpovidajici kanal */
        break;
      }
    }
    if (z_fail) {
      fprintf(stderr,"conti_measure: Channel %d is disable, not allowed use option -z %d,n!\n",z[j][0],z[j][0]);
      exit(EXIT_FAILURE);
    }
  }

/* Casova kontrola */
  time(&start);
  if (w==1) 
    end = start + 60*60*24; /* One day [s] */	
  else if (w==2) 
    end = start + 60*60*24*7; /* One week [s] */
  else 
    end = 0;

/* Print head of device responeses */
  for (k=0; k<f+1; k++) {
    fprintf(fp[k],"# Device type: %s\n",name);
    fprintf(fp[k],"# Device address: %02XH\n",adr);
    fprintf(fp[k],"# Number of print decimal digits: %d\n",res);
    fprintf(fp[k],"# Continual measure, dt = %.1lf s\n" , interval);
    for (i=0; i<itt; i++)
      fprintf(fp[k],"# Transformation channel Ch%d via table %d: %s %s\n",z[i][0],z[i][1],T.note[i],table_interval(z[i][1]));
    fprintf(fp[k],"#\n# Date                  ");
    for (i=0; i < nch; i++) {  
      fprintf(fp[k],"Ch %-6d",ch[i]);  
    }
    fprintf(fp[k],"\n");
    fflush(fp[k]); /* vyprazdni buffer */
  }

/* Nekonecny merici cyklus */
  for (;;) {
    if ( get_value(fd, adr, nch, val) == 0 ) { /* Precteni hodnot z ADAMa */
/* Tisk vysledku */
      for (k=0; k<f+1; k++) {
        fprintf(fp[k],"%s  ", now(0));  /* Print actual time */
        for (i=0; i < nch; i++) {
          for (j=0; j<itt; j++) {
            if (ch[i] == z[j][0]) {  /* Prepocet vybraneho kanalu podle tabulky z[1] */
              val[i] = interpolation(val[i],z[j][1]);
            }
          }
          (val[i] != 888888 && val[i] != -9999.9) ? fprintf(fp[k],"%-8.*f ",res,val[i]) : fprintf(fp[k],"NaN      "); /* Print 'res' frac. decimals */
        }
        fprintf(fp[k],"\n");
        fflush(fp[k]); /* vyprazdni buffer */
      }
    }
    time(&actual);
    if (w && actual > end) {
      return 0;	    /* Ukonceni mereni daneho cas. intervalu */
    }
/* Cekej [us] (vstup interval [s]) */
    usleep((useconds_t)(interval*1000000));
  }
  return -1;
}


/* Nacteni hodnot otevrenych kanalu (celkem 'nch')
 * Pri uspechu je navratova hodnota 0. Pri nezdaru cteni z ADAMa je navratova hodnota -1 */
static int get_value(int fd, unsigned char adr, int nch, double val[8])
{
  int i, j, k;
  int length;
  char token[LTMAX];
//  char *stest = ">+0001.2-1234.5-1234.5-1234.5-1234.5-1234.5-1234.5-1234.5"; /* ladici... */
  char buf[16]; /* Pole pro zapis cteneho cisla */
  double d;
  static char *msg = "get_value";  /* Message */

//  printf("token: #%02X\n",adr);
  sprintf(token,"#%02X",adr);  /* Formulace tokenu - dotaz na ADAMa */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Odpoved ADAMa */
    return -1;

//  printf("%s\n",token);

  length = strlen(token); /* Pocet znaku tokenu */
  i = k = 0;
  while (i < length && k < nch) {
    while (!(token[i] == '+' || token[i] == '-')) {  /* Preskoc znaky do vyskytu '+' nebo '-' */
      i++;
    }
    buf[0] = token[i++];  /* Zapis znamenko na pocatek bufferu */
    j = 1;
    while (isdigit(token[i]) || token[i]=='.')
      buf[j++] = token[i++];
    buf[j] = '\0';  /* Terminace bufferu */
    if ( sscanf(buf, "%lf", &d) == EOF ) {  /* Cti cislo z retezce */
//      perror(msg);
      fprintf (stderr, "%s: %s\n", msg, strerror(errno));
      exit(EXIT_FAILURE);
    }
    val[k++] = d;
  }
    return 0;
}


/* Fce vraci pocet des. mist na ktery se zaokrouhluje vystup v conti_measure
 * rozliseni podle typu ADAMa. Pri chybe je navratova hodnota -1, name is module name
 * Sampling frequency 7019R is about 15 Hz */
static int resolution(int fd, unsigned char adr, char *name)
{
  int res = 1; /* Default value */
  static char token[LTMAX];
  static char *buf[] = {"4015","4013","7015","7013","7019R",0}; /* Devices (ADAM or ICP_CON) with high resolution */
  char **p = buf;

  sprintf(token,"$%02XM",adr);  /* Dotaz na jmeno ADAMa */
  send_token(fd,token);  
  if (received_token(fd,token) < 0)   /* Odpoved ADAMa */
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
