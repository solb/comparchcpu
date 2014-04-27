#!/bin/sh

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: intermediate micro--jump table and control word micro-opcode generator

# output filenames
microcode="ucode.tu"
jumptable="jmptab.tl"
ctrlwords="ctrlwords.cwt"

# Handles special labels provided by RTL itself (namely whiles, elses, and fis)
processlangbranch() {
	echo "processing: $1$2"
	if "$3" ; then
		address=$(($address + 1))
		echo "$line" | sed -e "s/$1/&$2/" >>"$microcode"
	fi
	printf '%x\t%s\n' $address "$1$2" >>"$jumptable"
}

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
truncate -s 0 "$microcode"
labels=`echo "$listing" | sed -ne '/:/p' -e '/goes here/p'`
echo "$labels" >"$jumptable"

address=0
whiles=0
elses=0
fis=0
echo "$listing" | while read line ; do
	if ( echo "$line" | grep "while" >/dev/null 2>&1 ) ; then
		processlangbranch while $whiles false
		whiles=$(($whiles + 1))
	elif ( echo "$line" | grep "else" >/dev/null 2>&1 ) ; then
		processlangbranch else $elses true
		elses=$(($elses + 1))
	elif ( echo "$line" | grep "\<fi\>" >/dev/null 2>&1 ) ; then
		processlangbranch fi $fis false
		fis=$(($fis + 1))
	elif ( echo "$line" | grep ":" >/dev/null 2>&1 ) ; then
		name=`echo "$line" | sed 's/:*$//'`
		echo "processing: $name"
		sed -i "s/.*\(\<$name\>\).*/`printf %x $address`\t\1/" "$jumptable"
	elif ! ( echo "$line" | grep "goes here" >/dev/null 2>&1 ) ; then
		echo "$line" >>"$microcode"
		address=$(($address + 1))
	fi
done

words=`echo "$listing" | sed -e '/:/d' -e '/goes here/d' -e '/^#/d' -e '/if/d' -e '/else/d' -e '/fi/d' -e '/while/d' -e '/done/d' | sort | uniq`
truncate -s 0 "$ctrlwords"

uopcode=0
echo "$words" | while read line ; do
	printf '%02x\t%s\n' "$uopcode" "$line" >>"$ctrlwords"
	uopcode=$(($uopcode + 1))
done
