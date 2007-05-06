VERSION=1.7
PACKAGE=summon-$(VERSION).tar.gz
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu

install:
	$(MAKE) -C src/Summon/ install


pkg: dist/$(PACKAGE)
	

dist/$(PACKAGE):
	python setup.py sdist


pub: dist/$(PACKAGE)
	scp dist/$(PACKAGE) $(WWWHOST):$(WWWDIR)
	ssh $(WWWHOST) tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)


clean:
	$(MAKE) -C src/Summon/ cleanall
