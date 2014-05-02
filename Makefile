DIAGRAMS = inkscape -A $@ $<

documents: isa.pdf hard.pdf
	touch documents

hard.pdf: hard.tex datapath.pdf controlpath.pdf ucode/ctrlwords.tex ucode/ifclauses.tex
datapath.pdf: datapath.svg
	${DIAGRAMS}
controlpath.pdf: controlpath.svg
	${DIAGRAMS}

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
ucode/%: hard.tex
	${MAKE} -C ucode $(notdir $@)

clean:
	rm -f documents datapath.pdf controlpath.pdf *.aux *.log
wipe: clean
	rm -f *.pdf
