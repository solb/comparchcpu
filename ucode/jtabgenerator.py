#!/usr/bin/python3

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
