#!/usr/bin/python3

# Copyright (C) 2014 Sol Boucher and Evan Klei
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with it.  If not, see <http://www.gnu.org/licenses/>.

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: microcode generator

from sys import argv

INTER_EXT = '.tu'
FINAL_EXT = '.fu'

TYPE_OFFSET = 12
COND_OFFSET = 7

def main():
	if len(argv) != 5:
		print('USAGE: ' + argv[0] + ' <intermediate microcode> <control word mappings> <conditional mappings> <intermediate micro--jump table>' +
				'\nThe intermediate microcode should end in the extension ' + INTER_EXT +
				'\nThe final microcode\'s filename will end with ' + FINAL_EXT)
		return 1

	controls = establish_mappings(argv[2])
	conditions = establish_mappings(argv[3])
	labels = establish_mappings(argv[4], True)

	result = ''
	jtaddr = 0
	for line in open(argv[1]):
		result += '{0:x} 1 '.format(jtaddr)

		line = line.strip()
		if line[0] == '#':
			line = line[2:]
			lastspacer = line.rfind(' ')
			label = line[len(line) if lastspacer == -1 else lastspacer + 1:]
			line = line.replace(' ' + label, '')
		else:
			spacecadet = line.find(' ')
			poundcadet = line.find('#')
			if spacecadet == -1:
				spacecadet = poundcadet
			label = line[poundcadet + 1:] # The destination label, if one exists
			line = line[:len(line) if poundcadet == -1 else poundcadet]
			head = line[:spacecadet] # The first word
			tail = line[spacecadet + 1:] # Everything except the first word and the label

		if line in controls: # Control point
			result += '{0:x}\n'.format(controls[line])
		elif line == 'bail':
			result += '{0:x}\n'.format(0x6 << TYPE_OFFSET)
		elif line == 'bail out':
			result += '{0:x}\n'.format(0x7 << TYPE_OFFSET)
		elif line == 'print out':
			result += '{0:x}\n'.format(0x8 << TYPE_OFFSET)
		elif line == 'jump to the very':
			result += '{0:x}\n'.format(0x2 << TYPE_OFFSET)
		elif line == 'call function at uJumpTab label':
			result += '{0:x}\n'.format(0x4 << TYPE_OFFSET)
		elif line == 'call':
			result += '{0:x}\n'.format((0x3 << TYPE_OFFSET) | labels[label])
		elif line == 'return':
			result += '{0:x}\n'.format(0x5 << TYPE_OFFSET)
		elif head == 'if' or head == 'elif' or head == 'until':
			result += '{0:x}\n'.format((0xa << TYPE_OFFSET) | (conditions[tail.replace(' then', '').replace(' repeat', '')] << COND_OFFSET) | labels[label])
		elif head == 'else':
			result += '{0:x}\n'.format((0x1 << TYPE_OFFSET) | labels[label])
		else:
			print('WARNING: HIT AN UNMATCHED CASE!')

		jtaddr += 1
	result += '0'

	# Decide what to name the output file
	filename = argv[1]
	currext = filename.rfind(INTER_EXT)
	if currext != -1:
		filename = filename[:currext]
	filename += FINAL_EXT

	open(filename, 'w').write(result)

# Create a dictionary mapping human-readable stuff to numbers
# Arguments:
# 	filename   : the source filename
# 	indexasnum : whether the row index represents the value and the keys are in the second row
# 					(otherwise, there're assumed to be two columns, with decimal numbers in the second column)
def establish_mappings(filename, indexasnum=False):
	assoc = {}
	index = 0
	for line in open(filename):
		pair = line.strip().split('\t')
		if indexasnum:
			assoc[pair[1]] = index
			index += 1
		else:
			assoc[pair[0]] = int(pair[1])
	return assoc

if __name__=='__main__':
	main()
