IMAGES = 
PAPER = summon_manual
TEX = $(PAPER).tex
DVI = $(PAPER).dvi
PDF = $(PAPER).pdf


$(PDF): $(TEX) $(IMAGES) reference.tex
	latex $(TEX)
	dvipdf $(DVI)

bib:
	bibtex rearrange
	latex $(TEX)
	latex $(TEX)
	dvipdf $(DVI)

$(IMAGES) : %.pdf: %.eps
	epstopdf $<

reference.tex:
	summon ../examples/summon/help.py reference.tex > /dev/null

clean:
	rm -rf $(PDF) $(IMAGES)

