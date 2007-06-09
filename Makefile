#
# SUMMON - Linux Makefile
#

VERSION=1.7.1
PACKAGE=summon-$(VERSION).tar.gz
RPM=summon-$(VERSION)-1.i386.rpm
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu
PROG=summon_core.so
SRCPREFIX=src

include src/Makefile.in


all: lib/$(PROG)


# linking
ifdef PROFILE
    LIBS := $(LIBS) -pg
endif


LIBS := $(LIBS) -lglut -lGL -lSDL -lutil -lpython2.4 -pthread 



lib/$(PROG): 
	make -C src/Summon PROG=$(PROG) CFLAGS="$(CFLAGS)" LIBS="$(LIBS)"

clean: cleanall
	rm -rf lib/$(PROG)	


# create python package
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

