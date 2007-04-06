VERSION=1.6.1
PACKAGE=summon-$(VERSION).tar.gz
WWWDIR=~/public_html/summon/download

dist/$(PACKAGE):
	python setup.py sdist


pub: dist/$(PACKAGE)
	cp dist/$(PACKAGE) $(WWWDIR)
	tar zxvf $(WWWDIR)/$(PACKAGE) -C $(WWWDIR)
