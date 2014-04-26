#!/bin/sh

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: intermediate micro--jump table and control word micro-opcode generator

jumptable="jmptab.tl" # output file
ctrlwords="ctrlwords.cwt" # output file

if [ $# -ne 1 ] ; then
	cat <<-EOM
		USAGE: $0 <hardware reference document>
		The hardware reference document should end in the extension .tex
		The generated intermediate table will be named $jumptable
		The generated control words table will be named $ctrlwords
	EOM
	exit 1
fi

listing=`sed -n '/BEGIN RTL/,/END RTL/p' "$1" | sed -re '/^$/d' -e 's/^[ \t]+//' -e '/\{.*\}/d' -e '/^%/d' -e 's/([^[:space:]]) #.*$/\1/' -e '/^# [A-Z].*$/d'`
labels=`echo "$listing" | sed -ne '/:/p' -e '/goes here/p'`
echo "$labels" >"$jumptable"
words=`echo "$listing" | sed -e '/:/d' -e '/goes here/d' -e '/^#/d' -e '/if /d' -e '/else/d' | sort | uniq`
truncate -s 0 "$ctrlwords"

address=0
echo "$listing" | while read line ; do
	if ( echo "$line" | grep ":" >/dev/null 2>&1 ) ; then
		name=`echo "$line" | sed 's/:*$//'`
		echo "processing: $name"
		sed -i "s/.*\(\<$name\>\).*/`printf %x $address`\t\1/" "$jumptable"
	elif ! ( echo "$line" | grep "goes here" >/dev/null 2>&1 ) ; then
		address=$(($address + 1))
	fi
done

uopcode=0
echo "$words" | while read line ; do
	printf '%02x\t%s\n' "$uopcode" "$line" >>"$ctrlwords"
	uopcode=$(($uopcode + 1))
done
