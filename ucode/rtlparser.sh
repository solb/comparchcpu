#!/bin/sh

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: intermediate micro--jump table and control word micro-opcode generator

# output filenames
microtemp="ucode.tmp"
microcode="ucode.tu"
jumptable="jmptab.tl"
ctrlwdmap="ctrlwords.map"
ctrlwords="ctrlwords.tex"
ifclasmap="ifclauses.map"
ifclauses="ifclauses.tex"

# Handles special labels provided by RTL itself (namely whiles, elses, and fis)
# Arguments:
# 	base := while else fi
# 	num  :  int
# 	inst :  destination label identifier (if this is a line of code as well as a label)
processlangbranch() {
	echo "processing: $1$2"
	if [ -n "$3" ] ; then
		address=$(($address + 1))
		echo "$line#\$arr$3" >>"$microtemp"
	fi
	printf '%x\t%s\n' $address "$1$2" >>"$jumptable"
}

microcodewriteback() {
	truncate -s 0 "$microcode"
	cat "$microtemp" | while read line ; do
		poundindex=`expr index "$line" '#'`
		levelcode=`echo "$line" | sed -nr 's/^.*[^[:digit:]]([0-9]+)_.*$/\1/p'`
		[ -z "$levelcode" ] && levelcode=0
		if [ $poundindex -gt 1 -a $levelcode -eq $level ] ; then
			length=`expr length "$line"`
			frontend=`expr substr "$line" 1 $poundindex`
			backend=`expr substr "$line" $(($poundindex + 1)) $(($length - $poundindex))`
			if ( echo "$backend" | grep '_0$' >/dev/null 2>&1 ) ; then
				labelindex=`eval echo "$backend"`
				backend=`echo "$backend" | sed "s/_0$/_${labelindex}/"`
			fi
			backend=`eval echo "$backend"`
			echo "$frontend$backend" >>"$microcode"
		else
			echo "$line" >>"$microcode"
		fi
	done
	mv "$microcode" "$microtemp"
}

# Generates a mapping of opcodes to semantics
# Arguments:
# 	semantics : str (list of unique semantics)
#	mapfile   : filename
# 	texfile   : filename
assignopcodes() {
	truncate -s 0 "$2"
	truncate -s 0 "$3"
	uopcode=0
	echo "$1" | while read line ; do
		printf '%s\t%d\n' "$line" "$uopcode" >>"$2"
		escaped=`echo "$line" | sed -e 's/&/\\\&/' -e 's/\^/\\\^\\\/' -e 's/~/\\\~{ }/' -e 's/<-/$\\\leftarrow$/' -e 's/ - / -- /' -e 's/<</< </' -e 's/>>>/> > >/' -e 's/>>/> >/'`
		printf '0x%02x & %s \\\\\n' "$uopcode" "$escaped" >>"$3"
		uopcode=$(($uopcode + 1))
	done
}

uopcode=0
echo "$words" | while read line ; do
	printf '0x%02x & %s\n' "$uopcode" "$line" >>"$ctrlwords"
	uopcode=$(($uopcode + 1))
done

if [ $# -ne 1 ] ; then
	cat <<-EOM
		USAGE: $0 <hardware reference document>
		The hardware reference document should end in the extension .tex
		The generated intermediate table will be named $jumptable
		The generated control words table will be named $ctrlwords
	EOM
	exit 1
fi

# labels and "belongs here"s belong in the jump table
# elifs, elses, fis, and whiles belong in the jump table
# ifs, elses, and dones count as an instruction
# elifs count as two instructions (since they include a jump and a branch)

listing=`sed -n '/BEGIN RTL/,/END RTL/p' "$1" | sed -re '/^$/d' -e 's/^[ \t]+//' -e '/\{.*\}/d' -e '/^%/d' -e 's/([^[:space:]]) #.*$/\1/' -e '/^# [A-Z].*$/d' -e '/elif/ielse'`
truncate -s 0 "$microtemp"
labels=`echo "$listing" | sed -ne '/:/p' -e '/goes here/p'`
echo "$labels" >"$jumptable"

address=0
whiles=0
elses=0
fis=0
level=0
echo "$listing" | while read line ; do
	eval index=\$arr${level}_0
	if ( echo "$line" | grep "while" >/dev/null 2>&1 ) ; then
		level=$(($level + 1))
		eval arr${level}_1=while$whiles
		index=1
		processlangbranch while $whiles ""
		whiles=$(($whiles + 1))
	elif ( echo "$line" | grep "else" >/dev/null 2>&1 ) ; then
		eval arr${level}_$index=else$elses
		index=$(($index + 1))
		processlangbranch else $elses ${level}_0
		elses=$(($elses + 1))
	elif ( echo "$line" | grep "\<fi\>" >/dev/null 2>&1 ) ; then
		processlangbranch fi $fis ""
		eval arr${level}_$index=fi$fis
		microcodewriteback
		level=$(($level - 1))
		eval index=\$arr${level}_0
		fis=$(($fis + 1))
	elif ( echo "$line" | grep ":" >/dev/null 2>&1 ) ; then
		name=`echo "$line" | sed 's/:*$//'`
		echo "processing: $name"
		sed -i "s/.*\(\<$name\>\).*/`printf %x $address`\t\1/" "$jumptable"
	elif ! ( echo "$line" | grep "goes here" >/dev/null 2>&1 ) ; then
		if ( echo "$line" | grep "done" >/dev/null 2>&1 ) ; then
			echo "$line#\$arr${level}_1" >>"$microtemp"
			microcodewriteback
			level=$(($level - 1))
			eval index=\$arr${level}_0
		elif ( echo "$line" | grep "\<if\>" >/dev/null 2>&1 ) ; then
			level=$(($level + 1))
			index=1
			echo "$line#\$arr${level}_1" >>"$microtemp"
		elif ( echo "$line" | grep "elif" >/dev/null 2>&1 ) ; then
			echo "$line#\$arr${level}_$index" >>"$microtemp"
		else
			echo "$line" >>"$microtemp"
		fi
		address=$(($address + 1))
	fi
	eval arr${level}_0=$index
done
microcodewriteback
mv "$microtemp" "$microcode"

words=`echo "$listing" | sed -e '/:/d' -e '/goes here/d' -e '/^#/d' -e '/if/d' -e '/else/d' -e '/fi/d' -e '/while/d' -e '/done/d' | sort | uniq`
assignopcodes "$words" "$ctrlwdmap" "$ctrlwords"

clauses=`echo "$listing" | sed -ne 's/.*if \(.*\) then.*/\1/p' -e 's/.*while \(.*\) do.*/\1/p' | sort | uniq`
assignopcodes "$clauses" "$ifclasmap" "$ifclauses"
