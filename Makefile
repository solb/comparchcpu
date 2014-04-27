documents: isa.pdf hard.pdf
	touch documents

hard.pdf: hard.tex ucode/ctrlwords.tex ucode/ifclauses.tex

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
ucode/%:
	${MAKE} -C ucode $(notdir $@)

clean:
	rm -f *.aux *.log
wipe: clean
	rm -f documents *.pdf
