.SUFFIXES:	.tco

BIN = /data/lav/gaze/tlocal/ictools/itools/
ICC = $(BIN)/icc
ICCONF = $(BIN)/icconf
ILINK = $(BIN)/ilink
ICOLLECT = $(BIN)/icollect
ICCFLAGS = -g -DTRANSPUTER
ICCONF_FLAGS = -g

TCOFILES = root.tco lowlev2.tco zero.tco channel.tco flag.tco
HFILES = flag.h

all: gti.btl

gti.btl: gti.cfb
	$(ICOLLECT) gti.cfb

gti.cfb: gti.cfs root.lku
	$(ICCONF) $(ICCONF_FLAGS) gti.cfs

root.lku: root.tco lowlev2.tco zero.tco channel.tco
	$(ILINK) root.tco lowlev2.tco zero.tco channel.tco \
		-f startup.lnk -t800 -o root.lku

root.tco: root.c
	$(ICC) $(ICCFLAGS) root.c -t800

flag.tco: flag.c
	$(ICC) $(ICCFLAGS) flag.c -t800

lowlev2.tco: lowlev2.c
	$(ICC) $(ICCFLAGS) lowlev2.c -t800

zero.tco: zero.c
	$(ICC) $(ICCFLAGS) zero.c -t800

channel.tco: channel.c channel.h
	$(ICC) $(ICCFLAGS) channel.c -t800

realclean clean:
	rm *.tco *.cfb *.btl *.lku *~
