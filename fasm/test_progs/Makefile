OBJS := $(subst .fs,.fo,$(wildcard *.fs))

objects: ${OBJS}
	touch objects

%.fo: %.fs
	../fasm $<

wipe: clean
	rm -f ${OBJS}
clean:
	- rm objects
