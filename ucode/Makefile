INTERMEDIATES := ./rtlparser.sh ../hard.tex

jmptab.fl: jmptab.tl
	./jtabgenerator.py jmptab.tl

ctrlwords.map: ../hard.tex
	${INTERMEDIATES}
ctrlwords.tex: ../hard.tex
	${INTERMEDIATES}
ifclauses.map: ../hard.tex
	${INTERMEDIATES}
ifclauses.tex: ../hard.tex
	${INTERMEDIATES}
jmptab.tl: ../hard.tex
	${INTERMEDIATES}
ucode.tu: ../hard.tex
	${INTERMEDIATES}

clean:
	- rm ctrlwords.map ifclauses.map jmptab.tl ucode.tu
wipe: clean
	rm -f ctrlwords.tex ifclauses.tex jmptab.fl
