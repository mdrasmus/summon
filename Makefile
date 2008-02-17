#
# SUMMON - Linux Makefile
#

VERSION=1.8.5
MODULE=summon_core.so

all: lib/$(MODULE)

prefix=/usr/
PYTHON_PREFIX=$(prefix)/lib/python2.4/site-packages/

# include basic rules
SRCPREFIX=src
include src/Makefile.in


# profiling
ifdef PROFILE
    LDFLAGS := $(LDFLAGS) -pg
endif

# linking for summon module on Linux
LDFLAGS := $(LDFLAGS) -lglut -lGL -lSDL -lutil -lpython2.4 -pthread 

export MODULE CFLAGS LDFLAGS

# make rule for summon module on Linux
lib/$(MODULE): src/Summon/$(MODULE)	
	cp src/Summon/$(MODULE) lib

clean: cleanall
	rm -rf lib/$(MODULE)


install: lib/$(MODULE)
	cp -r lib/* $(PYTHON_PREFIX)
