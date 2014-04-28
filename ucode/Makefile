INTERMEDIATES := ./rtlparser.sh ../hard.tex

jmptab.fl: jmptab.tl
	./jtabgenerator.py jmptab.tl

ucode.tu: ../hard.tex
	${INTERMEDIATES}
jmptab.tl: ../hard.tex
	${INTERMEDIATES}

clean:
	- rm jmptab.tl
wipe: clean
	rm -f ucode.tu ctrlwords.tex ifclauses.tex jmptab.fl
