#
# SUMMON - Linux Makefile
#

# install prefix (can overload on command line)
prefix = /usr

# python version (can be 2.4 or 2.5)
PYTHON_VERSION = 2.4

# python install location
PYTHON_PREFIX = $(prefix)/lib/python$(PYTHON_VERSION)/site-packages/

# python include path
PYTHON_INCLUDE = $(prefix)/include/python$(PYTHON_VERSION)

# module name
MODULE = summon_core.so

# options for compiling and linking
LDFLAGS := $(LDFLAGS) -lglut -lGL -lSDL -lutil -lpython$(PYTHON_VERSION) -pthread 
CFLAGS := $(CFLAGS) -I$(PYTHON_INCLUDE) -fno-strict-aliasing

# programs
CC = g++
AR = ar
MAKE = make
RM = rm -rf
CP = cp -r

export MODULE CFLAGS LDFLAGS CC AR MAKE RM CP

#=============================================================================
# main target

all: lib/$(MODULE)

# include basic rules
SRCPREFIX=.
include Makefile.in

# top-level rules
lib/$(MODULE): src/Summon/$(MODULE)	
	$(CP) src/Summon/$(MODULE) lib

clean: cleanall
	$(RM) lib/$(MODULE)

install: lib/$(MODULE)
	$(CP) lib/* $(PYTHON_PREFIX)
