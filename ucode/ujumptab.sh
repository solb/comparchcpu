#!/bin/sh

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: intermediate micro--jump table generator

jumptable="tables.tl" # output file

if [ $# -ne 1 ] ; then
	cat <<-EOM
		USAGE: $0 <hardware reference document>
		The hardware reference document should end in the extension .tex
		The generated intermediate table will be named $jumptable
	EOM
	exit 1
fi

listing=`sed -n '/BEGIN RTL/,/END RTL/p' "$1" | sed -re '/^$/d' -e 's/^[ \t]+//' -e '/\{.*\}/d' -e '/^%/d' -e 's/([^[:space:]]) #.*$/\1/' -e '/^# [A-Z].*$/d'`
labels=`echo "$listing" | sed -ne '/:/p' -e '/goes here/p'`
echo "$labels" >labels.tl

address=0
echo "$listing" | while read line ; do
	if ( echo "$line" | grep ":" >/dev/null 2>&1 ) ; then
		name=`echo "$line" | sed 's/:*$//'`
		echo "processing: $name"
		sed -i "s/.*\(\<$name\>\).*/`printf %x $address`\t\1/" labels.tl
	elif ! ( echo "$line" | grep "goes here" >/dev/null 2>&1 ) ; then
		address=$(($address+1))
	fi
done
