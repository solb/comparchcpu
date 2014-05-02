#!/usr/bin/python3

# Sol Boucher and Evan Klei
# CSCI-453-01 Computer Architecture
# FabComp: microcode generator

from sys import argv

INTER_EXT = '.tu'
FINAL_EXT = '.fu'

def main():
	if len(argv) != 4:
		print('USAGE: ' + argv[0] + ' <intermediate microcode> <control word mappings> <conditional mappings>' +
				'\nThe intermediate microcode should end in the extension ' + INTER_EXT +
				'\nThe final microcode\'s filename will end with ' + FINAL_EXT)
		return 1

	controls = establish_mappings(argv[2])
	conditions = establish_mappings(argv[3])

	result = ''
	jtaddr = 0
	for line in open(argv[1]):
		result += '{0:x} 1 '.format(jtaddr)

		line = line.strip()
		if line in controls: # Control point
			result += '{0:x}\n'.format(controls[line])
		else:
			result += line + '\n'

		jtaddr += 1
	result += '0'

	# Decide what to name the output file
	filename = argv[1]
	currext = filename.rfind(INTER_EXT)
	if currext != -1:
		filename = filename[:currext]
	filename += FINAL_EXT

	open(filename, 'w').write(result)

def establish_mappings(filename):
	assoc = {}
	for line in open(filename):
		pair = line.strip().split('\t')
		assoc[pair[0]] = int(pair[1])
	return assoc

if __name__=='__main__':
	main()
