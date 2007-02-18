PACKAGE = summon
VERSION = 1.1

all:
	make -C src/Summon summon

windows:
	make -C src/Summon summon.exe
	cp src/Summon/summon bin

clean:
	make -C src/Summon cleanall

install:
	make -C src/Summon install
	
cleanobj:
	make -C src/Summon cleanallobj

dist:
	rm -rf ../$(PACKAGE)-$(VERSION)
	mkdir ../$(PACKAGE)-$(VERSION)
	cp -rv * ../$(PACKAGE)-$(VERSION)
	rm -rf $$(find ../$(PACKAGE)-$(VERSION) -name CVS)
	cd ../$(PACKAGE)-$(VERSION) && make cleanobj
	cd .. && tar zcvf $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION)
