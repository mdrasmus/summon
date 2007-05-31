IMAGES = 
PAPER = summon_manual
TEX = $(PAPER).tex
DVI = $(PAPER).dvi
PDF = $(PAPER).pdf


$(PDF): $(TEX) $(IMAGES) reference.tex
	pdflatex $(TEX)

toc:
	pdflatex $(TEX)
	pdflatex $(TEX)
        

$(IMAGES) : %.pdf: %.eps
	epstopdf $<

reference.tex: help.py
	python help.py > reference.tex

clean:
	rm -rf $(PDF) $(IMAGES)

cleanobj:
	rm -rf *.log *.toc *.aux
