/*
 *  Communication with data acqiusition modules via protocol DCON
 *
 *  V1.7/2020-02-07 Add multi -z parameter for transformation tables
 *  V1.10/2020-03-19
 *  V1.11/2020-06-02 Add 'res' to options
 *  V1.13/2020-06-12 Change in -w options
 *  V1.14/2020-06-21 Rename of data file (-f parameter)
 */


#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <errno.h>
#include <libgen.h>
#include <string.h>

#include "serial.h"
#include "terminal.h"
#include "time.h"
#include "measure.h"
#include "revision.h"
#include "scan.h"
#include "channels.h"
#include "typedef.h"
#include "read_table.h"
#include "table_interval.h"
#include "read_sensor_names.h"

/* Default port */
#define PORT "/dev/ttyUSB0"  /* Linux USB serial */
//#define PORT "/dev/ttyU0"  /* OpenBSD USB serial */
// #define PORT "/dev/cua00"  /* OpenBSD COM0 */
#define LTMAX 128  /* Maximal length of token (include '\0') */
#define CR '\r' /* Terminal character */
#define ADR '\x01'  /* Device default adress */
#define VERBOSE 0  /* Print calibration tables */

/*
 *  Global variable
 */
char *progname;
TRANS_TAB T;  /* Transformation table */
char *sensors[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; /* Names of senssors at channels */
char *dev_name = NULL; /* Device name specifed in command line */

int init(char *device)
{
    int fd;  /* File descriptor */
    fd = open_port (device);
    set_port (fd);
    return fd;
}

void usage(void)
{
    fprintf(stderr, "usage: %s [options and parameters]\n\n" ,progname);
}

void help(void)
{
  static char *msg[] = {"-p [name]\tPort name (default: "PORT"), e.g. /dev/ttyU0, /dev/cua00",
	                "-a [addr]\tDevice address (dec)",
			"-i [dt]\t\tTime interval [s]",
			"-c [code]\tEnable channels, code = ch[0]+2ch[1]+4ch[2]+...+128ch[7]\n\t\texample: code = 63 for enable channels ch[0]..ch[5]",
                        "-t\t\tTerminal for manual type commands",
			"-s [addr]\tScan address from 'addr' to 255",
			"-w [n]\t\tTime interval for measure {0-ever, 1-min, 2-hour, 3-day, 4-week}",
                        "-z [m,n]\tApply transf. table n to channel m (Lagrange interpol. from table)",
                        "-f\t\tWrite output to terminal and file in format: aa_yymmddhhmmss.dat",
                        "-r [n]\t\tNumber of decimal places after dot in output values",
                        "-d\t\tRead names of ID sensors from file 'IDSensors.dat'",
                        "-x [name]\t'name' is printed in output (device)",
                        "-h or -?\tHelp",
	                0 };
  char **p = msg;
  fprintf(stderr,"%s, DCON. Version %s, %s\n\n", progname, VERSION, REVDATE);
  usage();
  while (*p) fprintf(stderr, "%s\n", *p++);
}


int main(int argc, char *argv[])
{
  char *port = NULL;  /* Port name */
  int fd;  /* File descriptor */
  int c; /* Character */
  int term = 0;
  int scan = 0;
  unsigned char adr = ADR;  /* Device address */
  double dt = 1;  /* Time period for data read [s] */
/* Zapnuti vybranych kanalu, nc = ch[0]+2ch[1]+4ch[2]+...+128ch[7], 1-on, 0-off
 * nc = 0 .. k zadnemu nastaveni nedojde  */
  int nc = 0; /* None set channels */
  int w = 0; /* Time conti measure 0-ever, 1-day, 2-week*/
  int z[8][2]; /* z[0] channel, z[1] transformation table */
  int itt=0; /* Index (and finally number) of transformation table */
  int f=0; /* Write output to file is dissable */
  int res = 2; /* Default resolution (Number of decimal places after dot) */

  char sen_name[] = "IDSensors.dat"; /* */
  char cal_name[] = "calibration.dat";

/* Initialization z array */
  {
    int i,j;
    for(i=0;i<8;i++)
      for(j=0;j<2;j++)
        z[i][j] = -1;
  }  

  progname = basename(argv[0]);

  if (argc == 1) {
    printf("%s: For help, type '%s -h or -?'.\n",progname, progname);
  }

/* Parameteres in command line */
  while ( (c = getopt(argc, argv, "p:a:i:c:s:tw:z:fr:dx:h?")) != -1 ) {
//  printf("optind, argc: %d %d \n", optind, argc);
    switch (c) {
      case 'p':  /* Set port */
        port = optarg;
        break;
      case 'a':  /* Set address */
        adr = (unsigned char)atoi(optarg);
        break;
      case 'i': /* Time period of data read [s] */
        dt  = atof(optarg);
        break;
      case 'c':  /* Select channels */
        nc = atoi(optarg);
        if (nc < 1 || nc > 255) {
          fprintf (stderr, "%s: nc != <1..255>\n",progname);
          exit(EXIT_FAILURE);
        }
        break;
      case 't':
        term = 1;
        break;
      case 's':
        adr = (unsigned char)atoi(optarg);
	scan = 1;
	break;
      case 'w':
        w = atoi(optarg);
        break;
      case 'z':

        T = read_table(cal_name);

        #if VERBOSE
        for (int i=0;i<T.tables;i++) {
          printf("Table %d: %s\n",i,T.note[i]);
          printf("%s\n\n",table_interval(i));
        }
        #endif

        sscanf(optarg,"%d, %d", &z[itt][0], &z[itt][1]);
        if (z[itt][0] < 0 || z[itt][0]>7) {  /* Channel */
          fprintf (stderr, "%s: Incorrect parameter -z %d,n!\n",progname,z[itt][0]);
          exit(EXIT_FAILURE);
        }
        if (z[itt][1] < 0 || z[itt][1] >= T.tables) {  /* Table */
          fprintf (stderr, "%s: Incorrect parameter -z n,%d!\n",progname,z[itt][1]);
          exit(EXIT_FAILURE);
        }
        itt++;
        if (itt > 7) {
          fprintf (stderr, "%s: Too much -z parameters!\n",progname);
          exit(EXIT_FAILURE);
        }
        break; 
      case 'f':
        f = 1; /* Write output to file is enable */
        break;   
      case 'r': /* Resolution (Number of decimal places after dot) */
        res = atoi(optarg);
        break;
      case 'd':  /* Read sensor names from file */
        read_sensor_names(sen_name, sensors);
        break;
      case 'x': /* Read device name from command line */
        dev_name = strdup(optarg);
        break;
      case 'h':
        help();
        exit(EXIT_SUCCESS);
      case '?':
        help();
        exit(EXIT_SUCCESS);
      default:
	help();
        exit(EXIT_FAILURE);
    }
  }

///*tests */
//  {
//    int i;
//    for (i=0; i<itt; i++) 
//      printf("z[%d][0] = %d, z[%d][1] = %d\n",i,z[i][0],i,z[i][1]);
//  }  
//  exit(0);

/* Open port */
  if ( port == NULL )
    fd = init(PORT);  /* Default name */
  else
    fd = init(port);  /* Name */

/* Set of channels */
  if (nc > 0) {
    if (set_channels(fd, adr, nc) < 0) {
      fprintf(stderr,"Channel set failure...\n");
      exit(EXIT_FAILURE);	      
    }
    exit(EXIT_SUCCESS);
  }

/* Terminal */
  if (term) {
    terminal(fd);
    close(fd);
    return 0;
  }

/* Scan address space */
  if (scan) {
    adam_scan(fd,adr);
    close(fd);
    return 0;
  }

/* Default - measure */
  return conti_measure(fd, adr, dt, w, itt, z, f, res);
}
