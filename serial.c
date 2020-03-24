/*
 *  Function for open and set of parameter communication on seriial port
 *  V1/7.11.2012/TCh
 */
#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>  /* Standard lib */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

#include "serial.h"

#define BAUD_RATE B9600

/*
 *  Otevreni portu
 */
int open_port ( char *device )
{
  int fd; /* File descriptor for the port */

  if ((fd = open (device, O_RDWR | O_NOCTTY | O_NDELAY)) < 0) {;
/*
 *  flag O_NOCTTY - terminal nebude zasahovat do komunikace,
 *  vyuziva getty apod. programy, pro bezne pouziti urcite uvest.
 *
 *  O_NDELAY - nestarat se o signal DCD (Data Carrier Detect)
 *  jestlize se neuvede, program ceka az se DCD dostane na SPACE stav (+12V)
 */
    fprintf (stderr, "\nopen_port: Unable to open %s - %s\n", device, strerror(errno));
    exit (EXIT_FAILURE);
  }
  else if (fcntl (fd, F_SETFL, 0) == -1) {
/*
 *  fcntl(fd, F_SETFL, FNDELAY);
 *  FNDELAY - read vraci 0, jestli neni na portu zadny znak, do normalniho
 *  blokoveho rezimu kdy se ceka nez znak prijde se misto FNDELAY pouzije 0
 */
/*  perror ("open_port: fcntl error"); */
    fprintf (stderr, "\nopen_port: fcntl error %s\n", strerror(errno));
    exit (EXIT_FAILURE);
  }
  return fd;
}


/*
 *  Nastaveni serioveho port na rychlot BAUD_RATE do
 *  blokovaciho rezimu, kdyz v bufferu nejsou data dele nez 1 sekundu
 */
void set_port (int fd)
{
  struct termios options;  /* Systemova struktura terminalu */
/*
 *  Get the current options for the port
 */
  if (tcgetattr (fd, &options) == -1) {
/* perror ("set_port: tcgetattr failed"); */
    fprintf (stderr, "\nset_port: tcgetattr failed %s\n", strerror(errno));
    exit (EXIT_FAILURE);
  }
/*
 *  Vstupni rychlost portu
 */
  if (cfsetispeed (&options, BAUD_RATE) == -1) {
/* perror ("set_port: invalid baud rate (cfsetispeed)"); */
    fprintf (stderr, "\nset_port: invalid baud rate (cfsetispeed) %s\n", strerror(errno));
    exit (EXIT_FAILURE);
  }
/*
 *  Vystupni rychlost portu
 */
  if (cfsetospeed (&options, BAUD_RATE) == -1) {
/* perror ("set_port: invalid baud rate (cfsetospeed)"); */
    fprintf (stderr, "\nset_port: invalid baud rate (cfsetospeed) %s\n", strerror(errno));
    exit (EXIT_FAILURE);
  }
/*
 *  Non canonical input - nejsou zadne ridici znaky jen "raw data"
 *  Chovani spojeni urcuji parametry: VMIN a VTIME
 *  CS8 - 8 data bits
 */
  cfmakeraw (&options);
/*
 *  VMIN: Minimalni pocet znaku prijimany v read(), Napr. pro 0 je read ukonceno
 *        pri kazdem znaku nebo prekroceni VTIME
 */
  options.c_cc[VMIN] = 0;
/*
 *  VTIME: casovac, jak dlouho [1/10 sec] se muze cekat na znak nebo skupinu
 */
  options.c_cc[VTIME] = 15;
/*
 *  Set the new options for the port
 */
  if (tcsetattr (fd, TCSAFLUSH, &options) == -1) {
    fprintf (stderr, "\nset_port: tcsetattr failed %s\n", strerror(errno));
    exit (EXIT_FAILURE);
  }
}
