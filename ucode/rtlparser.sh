#!/bin/sh

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: intermediate micro--jump table and control word micro-opcode generator

jumptable="jmptab.tl" # output file
ctrlwords="ctrlwords.cwt" # output file

# Handles special labels provided by RTL itself (namely whiles, elses, and fis)
processlangbranch() {
	echo "processing: $name"
	printf '%x\t%s\n' $address "$name" >>"$jumptable"
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
labels=`echo "$listing" | sed -ne '/:/p' -e '/goes here/p'`
echo "$labels" >"$jumptable"
words=`echo "$listing" | sed -e '/:/d' -e '/goes here/d' -e '/^#/d' -e '/if/d' -e '/else/d' -e '/fi/d' -e '/while/d' -e '/done/d' | sort | uniq`
truncate -s 0 "$ctrlwords"
#listing=`echo "$listing" | sed '/elif/ielse'` TODO remove

#echo "$listing"
#exit 2

address=0
whiles=0
elses=0
fis=0
echo "$listing" | while read line ; do
	if ( echo "$line" | grep ":" >/dev/null 2>&1 ) ; then
		name=`echo "$line" | sed 's/:*$//'`
		echo "processing: $name"
		sed -i "s/.*\(\<$name\>\).*/`printf %x $address`\t\1/" "$jumptable"
	elif ( echo "$line" | grep "while" >/dev/null 2>&1 ) ; then
		name="while$whiles"
		processlangbranch
		whiles=$(($whiles + 1))
	elif ( echo "$line" | grep "else" >/dev/null 2>&1 ) ; then
		name="else$elses"
		address=$(($address + 1))
		processlangbranch
		elses=$(($elses + 1))
	elif ( echo "$line" | grep "fi" >/dev/null 2>&1 ) ; then
		name="fi$fis"
		processlangbranch
		fis=$(($fis + 1))
	elif ! ( echo "$line" | grep "goes here" >/dev/null 2>&1 ) ; then
		address=$(($address + 1))
	fi
done

uopcode=0
echo "$words" | while read line ; do
	printf '%02x\t%s\n' "$uopcode" "$line" >>"$ctrlwords"
	uopcode=$(($uopcode + 1))
done
