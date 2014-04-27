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
ucode/%:
	${MAKE} -C ucode $(notdir $@)

clean:
	rm -f *.aux *.log datapath.pdf
wipe: clean
	rm -f documents *.pdf
