VERSION=1.6.1
PACKAGE=summon-$(VERSION).tar.gz
WWWDIR=/scratch-sm/www/pub/summon
WWWHOST=rasmus@compbio.mit.edu


dist/$(PACKAGE):
	python setup.py sdist


pub: dist/$(PACKAGE)
	scp dist/$(PACKAGE) $(WWWHOST):$(WWWDIR)
	ssh $(WWWHOST) tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)

install:
	make -C src/Summon/ install

clean:
	make -C src/Summon/ cleanall
