#
# Makefile 
# V1.1/2013-05-01
# V1.2/2014-12-06 Add comments
# V1.3/2015-06-15
# V1.4/2016-1-19 Add time control
# V1.5/2019-11-10 Add terminal help
# V1.8/2020-02-10 Add parameter -f
# V1.9/2020-03-18 Read calibration tables from file, rename adam to dcon
#

program=dcon
VERS = 1.10

# Seznam souboru
OBJ=serial.o terminal.o io_token.o time.o measure.o scan.o channels.o terminal_help.o interpolation.o read_table.o decomment.o table_interval.o main.o
SRC=serial.c terminal.c io_token.c time.c measure.c scan.c channels.c terminal_help.c interpolation.c read_table.c decomment.c table_interval.c main.c
HEAD=serial.h terminal.h io_token.h time.h measure.h scan.h channels.h terminal_help.h interpolation.h read_table.h decomment.h table_interval.h typedef.h revision.h

# Prekladac C
CC = clang

# Mira optimalizace gcc (-O0 -g = debug, -O0 -pg = gprof, -O2 = normal)
OPT = -O2

# Ostatni parametry prekladace (-Wall -Wextra -pedantic)
CFLAGS = -Wall -Wextra #-pedantic

# Cilum build, install, uninstall, clean a distrib neodpovida primo zadny soubor
# (predstirany '.PHONY' target)

.PHONY: build
.PHONY: install
.PHONY: uninstall
.PHONY: clean
.PHONY: dist

# list of valid suffixes through the use of the .SUFFIXES special target.
# Pripony transformovanych soboru: s1.s2. jest  *.s1 --> *s2
.SUFFIXES: .c .o

# Prvni cil je implicitni, neni treba volat 'make build', staci 'make'.
# Cil build nema zadnou akci, jen zavislost.

build: $(program)

install: build
	cp $(program) /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(program)

clean:
	rm -f *.o $(program)

dist:
	tar czf $(program)-$(VERS).tgz $(SRC) $(HEAD) Makefile dcon_start.sh dcon_proc.sh dcon_kill.sh phrases.txt README

$(program): $(OBJ) Makefile
	$(CC) $(OBJ) -o $(program) $(OPT)

# The target form '.s1.s2', where .s1 and .s2 are currently valid suffixes, then
# it defines a transformation from *.s1 to *.s2 (double suffix interference)
# If a target has the form '.s1', where .s1 is a currently valid suffix,
# then it defines a transformation from *.s1 to * (single suffix interference)
#
# <  The name of the prerequisite from which this target is to be built,
#    if a valid inference rule (suffix rule) is in scope.

# Zavislost objektovych souboru, v BSD make je tato syntax problem...
#%.o: %.c $(HEAD) Makefile
#	$(CC) $(CFLAGS) $(OPT) -c $<

# Zavislost objektovych souboru (v BSD make funkcni)
.c.o: $(HEAD) Makefile
	$(CC) $(CFLAGS) $(OPT) -c $<
