IMAGES = 
PAPER = summon_manual
TEX = $(PAPER).tex
PDF = $(PAPER).pdf

all: toc html


toc:
	pdflatex $(TEX)
	pdflatex $(TEX)
        
html:
	./make.sh


$(PDF): $(TEX) $(IMAGES)
	pdflatex $(TEX)

$(IMAGES) : %.pdf: %.eps
	epstopdf $<

clean:
	rm -rf $(PDF) $(IMAGES)

cleanobj:
	rm -rf *.log *.toc *.aux
