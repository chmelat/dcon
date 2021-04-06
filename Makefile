#
# Makefile 
#

program=dcon
VERS = 1.15

# Seznam souboru
SRC=serial.c terminal.c io_token.c time.c measure.c scan.c channels.c terminal_help.c interpolation.c read_table.c decomment.c table_interval.c read_sensor_names.c main.c
OBJ=$(SRC:.c=.o)
HEAD=serial.h terminal.h io_token.h time.h measure.h scan.h channels.h terminal_help.h interpolation.h read_table.h decomment.h table_interval.h read_sensor_names.h typedef.h revision.h

# C translator (clang, gcc, ..)
CC = gcc

# Optimalization (-O0 -g = debug, -O0 -pg = gprof, -O2 = normal)
OPT = -O2

# Other parameters (-Wall -Wextra -pedantic)
CFLAGS = -Wall -Wextra #-pedantic

# Cilum build, install, uninstall, clean a dist neodpovida primo zadny soubor
# (predstirany '.PHONY' target)

.PHONY: build
.PHONY: install
.PHONY: uninstall
.PHONY: clean
.PHONY: dist

# list of valid suffixes through the use of the .SUFFIXES special target.
.SUFFIXES: .c .o

build: $(program)

install: build
	cp $(program) /usr/local/bin
	cp dcon.sh /etc/init.d
	cp dcon_start_tmpfs.sh /usr/local/bin
	cp IDSensors.dat /usr/local/bin
	cp calibration.dat /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(program)

clean:
	rm -f *.o $(program)

dist:
	tar czf $(program)-$(VERS).tgz $(SRC) $(HEAD) Makefile dcon.sh dcon_start.sh dcon_start_tmpfs.sh\
	  dcon_proc.sh dcon_kill.sh README.md calibration.dat IDSensors.dat

$(program): $(OBJ) Makefile
	$(CC) $(OBJ) -o $(program) $(OPT)

#%.o: %.c $(HEAD) Makefile
#	$(CC) $(CFLAGS) $(OPT) -c $<

# Zavislost objektovych souboru (v BSD make funkcni)
.c.o: $(HEAD) Makefile
	$(CC) $(CFLAGS) $(OPT) -c $<
