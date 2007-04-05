VERSION=1.6


dist/summon-$(VERSION).tar.gz:
	python setup.py sdist


pub: dist/summon-$(VERSION).tar.gz
	cp summon-$(VERSION).tar.gz ~/public_html/summon/download
	tar -C ~/public_html/summon/download zxvf summon-$(VERSION).tar.gz
