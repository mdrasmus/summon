VERSION=1.7.1
PACKAGE=summon-$(VERSION).tar.gz
RPM=summon-$(VERSION)-1.i386.rpm
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu

install:
	$(MAKE) -C src/Summon/ install


pkg: 
	make dist/$(PACKAGE)
	

dist/$(PACKAGE):
	rm -f MANIFEST
	python setup.py sdist
	python setup.py bdist --format=rpm


pub: dist/$(PACKAGE)
	scp dist/$(PACKAGE) $(WWWHOST):$(WWWDIR)
	scp dist/$(RPM) $(WWWHOST):$(WWWDIR)
	ssh $(WWWHOST) chmod o+r $(WWWDIR)/$(PACKAGE)
	ssh $(WWWHOST) chmod o+r $(WWWDIR)/$(RPM)
	ssh $(WWWHOST) tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)
	ssh $(WWWHOST) chmod -R o+rx $(WWWDIR)/summon-$(VERSION)

clean:
	$(MAKE) -C src/Summon/ cleanall
