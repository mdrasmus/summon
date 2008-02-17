#
# SUMMON - Linux Makefile
#

VERSION=1.8.5
MODULE=summon_core.so

prefix=/usr/
PYTHON_PREFIX=$(prefix)/lib/python2.4/site-packages/

CC = g++
AR = ar
MAKE = make
RM = rm -rf
CP = cp -r

all: lib/$(MODULE)

# profiling
ifdef PROFILE
    LDFLAGS := $(LDFLAGS) -pg
endif

# linking for summon module on Linux
LDFLAGS := $(LDFLAGS) -lglut -lGL -lSDL -lutil -lpython2.4 -pthread 

CFLAGS := $(CFLAGS) -I/usr/include/python2.4

export MODULE CFLAGS LDFLAGS CC MAKE RM CP

# include basic rules
SRCPREFIX=src
include src/Makefile.in

# make rule for summon module on Linux
lib/$(MODULE): src/Summon/$(MODULE)	
	$(CP) src/Summon/$(MODULE) lib

clean: cleanall
	$(RM) lib/$(MODULE)


install: lib/$(MODULE)
	$(CP) lib/* $(PYTHON_PREFIX)
