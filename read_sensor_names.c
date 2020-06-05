/*
 *  Read sensor names from file
 *  return value is number of readed devices
 *
 *  V0.1/2020-06-02
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decomment.h"

int read_sensor_names(const char *name_sensors, char *sensors[8]) {
  FILE *fd_in;
  char *line_buf = NULL;
  size_t line_buf_size = 0;
  ssize_t line_size;
  int ch; /* Channel 0..7 */
  char s[32];
  int n=0; /* Number of sensors */

/* Decomment input plain file */
  fd_in = decomment(name_sensors);

/* Get the first line of the file. */
  line_size = getline(&line_buf, &line_buf_size, fd_in);

/* Loop through until we are done with the file. */
  while (line_size >= 0)
  {
    n++;
    /* Show the line details */
//    printf("chars=%06zd, buf size=%06zu, contents: %s", line_size, line_buf_size, line_buf);
    sscanf(line_buf,"%d %s",&ch,s);

    sensors[ch] = strdup(s);

    /* Get the next line */
    line_size = getline(&line_buf, &line_buf_size, fd_in);
  }

/* Free the allocated line buffer */
  free(line_buf);
  line_buf = NULL;

  /* Close the file now that we are done with it */
  fclose(fd_in);

  return n;
}
