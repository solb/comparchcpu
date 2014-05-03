INTERMEDIATES := ./rtlparser.sh ../hard.tex

micromemories: ucode.fu jmptab.fl
	touch micromemories

ucode.fu: ucode.tu ctrlwords.map ifclauses.map jmptab.tl
	./ucodegenerator.py ucode.tu ctrlwords.map ifclauses.map jmptab.tl
jmptab.fl: jmptab.tl
	./jtabgenerator.py jmptab.tl

ctrlwords.tex: ../hard.tex
	${INTERMEDIATES}
ifclauses.tex: ../hard.tex
	${INTERMEDIATES}

ctrlwords.map: ../hard.tex
	${INTERMEDIATES}
ifclauses.map: ../hard.tex
	${INTERMEDIATES}
jmptab.tl: ../hard.tex
	${INTERMEDIATES}
ucode.tu: ../hard.tex
	${INTERMEDIATES}

clean:
	- rm micromemories ctrlwords.map ifclauses.map jmptab.tl ucode.tmp ucode.tu
wipe: clean
	rm -f ctrlwords.tex ifclauses.tex jmptab.fl ucode.fu
