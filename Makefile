#
# SUMMON - Linux Makefile
#

VERSION=1.8.1
MODULE=summon_core.so

all: lib/$(MODULE)

# include basic rules
SRCPREFIX=src
include src/Makefile.in


# profiling
ifdef PROFILE
    LIBS := $(LIBS) -pg
endif

# linking for summon module on Linux
LIBS := $(LIBS) -lglut -lGL -lSDL -lutil -lpython2.4 -pthread 

export MODULE CFLAGS LIBS

# make rule for summon module on Linux
lib/$(MODULE): src/Summon/$(MODULE)	
	cp src/Summon/$(MODULE) lib

clean: cleanall
	rm -rf lib/$(MODULE)


