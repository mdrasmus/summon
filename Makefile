VERSION=1.7
PACKAGE=summon-$(VERSION).tar.gz
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu

install:
	$(MAKE) -C src/Summon/ install


pkg: 
	make dist/$(PACKAGE)
	

dist/$(PACKAGE):
	rm -f MANIFEST
	python setup.py sdist


pub: dist/$(PACKAGE)
	scp dist/$(PACKAGE) $(WWWHOST):$(WWWDIR)
	ssh $(WWWHOST) chmod o+r $(WWWDIR)/$(PACKAGE)
	ssh $(WWWHOST) tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)


clean:
	$(MAKE) -C src/Summon/ cleanall
