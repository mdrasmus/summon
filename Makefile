#
# SUMMON - Linux Makefile
#

VERSION=1.7.2
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


#=============================================================================
# packaging 
PACKAGE=summon-$(VERSION).tar.gz
RPM=summon-$(VERSION)-1.i386.rpm
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu

# python package
pkg: 
	make dist/$(PACKAGE)


dist/$(PACKAGE):
	rm -f MANIFEST
	python setup.py sdist
	python setup.py bdist --format=rpm


# publish package to website
pub: dist/$(PACKAGE)
	scp dist/$(PACKAGE) $(WWWHOST):$(WWWDIR)
	scp dist/$(RPM) $(WWWHOST):$(WWWDIR)
	ssh $(WWWHOST) chmod o+r $(WWWDIR)/$(PACKAGE)
	ssh $(WWWHOST) chmod o+r $(WWWDIR)/$(RPM)
	ssh $(WWWHOST) tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)
	ssh $(WWWHOST) chmod -R o+rx $(WWWDIR)/summon-$(VERSION)

