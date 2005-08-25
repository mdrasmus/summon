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
