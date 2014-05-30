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
# FabComp: micro--jump table generator

from sys import argv

INTER_EXT = '.tl'
FINAL_EXT = '.fl'

def main():
	if len(argv) != 2:
		print('USAGE: ' + argv[0] + ' <intermediate micro--jump table>' +
				'\nThe intermediate table should end in the extension ' + INTER_EXT +
				'\nThe final table\'s filename will end with ' + FINAL_EXT)
		return 1

	result = ''
	jtaddr = 0
	for line in open(argv[1]):
		result += '{0:x} 1 {1}\n'.format(jtaddr, line[:line.find('\t')])
		jtaddr += 1
	result += '0'

	# Decide what to name the output file
	filename = argv[1]
	currext = filename.rfind(INTER_EXT)
	if currext != -1:
		filename = filename[:currext]
	filename += FINAL_EXT

	open(filename, 'w').write(result)

if __name__=='__main__':
	main()
