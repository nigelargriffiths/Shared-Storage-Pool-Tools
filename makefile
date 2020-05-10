CFLAGS= -g -O3

all: nslim nsspconf

nslim:
	cc $(CFLAGS) -o nslim nslim_v4.c $(LDFLAGS) 

nsspconf:
	@ echo Compiling on AIX 
	@ oslevel -s
	@ echo For VIOS 2.2.+ compile on AIX 6.1 TL9+
	@ echo For VIOS 3.+   compile on AIX 7.2 TL3+
	cc $(CFLAGS) -o nsspconf nsspconf.c $(LDFLAGS) -lperfstat

