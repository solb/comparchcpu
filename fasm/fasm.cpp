// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: assembler

#include <boost/algorithm/string/split.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
using boost::split;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::make_pair;
using std::min;
using std::stoi;
using std::string;
using std::transform;
using std::unordered_map;
using std::vector;

// File extensions
const char ASM_EXT[] = ".fs";
const char OBJ_EXT[] = ".fo";

// Instruction mnemonic -> opcode translations
const unordered_map<string, unsigned> OPC = {make_pair("halt",	0x00),

											 make_pair("and",	0x01),
											 make_pair("or",	0x02),
											 make_pair("xor",	0x03),
											 make_pair("lsft",	0x04),
											 make_pair("nand",	0x05),
											 make_pair("nor",	0x06),
											 make_pair("xnor",	0x07),
											 make_pair("rsft",	0x08),
											 make_pair("land",	0x09),
											 make_pair("lor",	0x0a),
											 make_pair("lxor",	0x0b),
											 make_pair("rasft", 0x0c),
											 make_pair("lnand", 0x0d),
											 make_pair("lnor",	0x0e),
											 make_pair("lxnor",	0x0f),
											 make_pair("slt",	0x10),
											 make_pair("sgt",	0x11),
											 make_pair("seq",	0x12),
											 make_pair("sne",	0x13),
											 make_pair("sle",	0x14),
											 make_pair("sge",	0x15),
											 make_pair("add",	0x16),
											 make_pair("sub",	0x17),

											 make_pair("blt",	0x18),
											 make_pair("bgt",	0x19),
											 make_pair("beq",	0x1a),
											 make_pair("bne",	0x1b),
											 make_pair("ble",	0x1c),
											 make_pair("bge",	0x1d),

											 make_pair("siz",	0x20),
											 make_pair("snz",	0x21),
											 make_pair("not",	0x22),
											 make_pair("neg",	0x23),

											 make_pair("biz",	0x24),
											 make_pair("bnz",	0x25),

											 make_pair("incr",	0x26),
											 make_pair("decr",	0x27),

											 make_pair("jmp",	0x28),
											 make_pair("jal",	0x29),
											 make_pair("call",	0x2a),

											 make_pair("ret",	0x2b),

											 make_pair("mov",	0x2c)};

// Register mnemonic -> address translations
const unordered_map<string, unsigned> REG = {make_pair("a0",	0x0),
											 make_pair("a1",	0x1),
											 make_pair("a2",	0x2),
											 make_pair("v",		0x3),
											 make_pair("s0",	0x4),
											 make_pair("s1",	0x5),
											 make_pair("s2",	0x6),
											 make_pair("s3",	0x7),
											 make_pair("t0",	0x8),
											 make_pair("t1",	0x9),
											 make_pair("t2",	0xa),
											 make_pair("t3",	0xb),
											 make_pair("t4",	0xc),
											 make_pair("t5",	0xd),
											 make_pair("sp",	0xe),
											 make_pair("ra",	0xf)};

// Left shifts for the opcode byte
const unsigned OPC_SHIFT = 8;
const unsigned IMM_SHIFT[] = {7, 6};
const unsigned ADM_SHIFT[] = {4, 2, 0};

// Left shifts for S-type immediate byte
const unsigned SIMM_SHIFT[] = {8, 0};

// Left shifts for R-type immediate byte
const unsigned SADM_SHIFT = 12;
const unsigned REG_SHIFT[] = {8, 4, 0};

// Label -> address mappings
unordered_map<string, unsigned> symb_tab;

// Label -> references mappings
unordered_map<string, list<unsigned>> rel_tab;

// The binary that we're generating
vector<unsigned> cs;

bool proc_line(unsigned, const string &);
bool error(unsigned, const char *);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "USAGE: " << argv[0] << " <assembly file>" <<
				"\nThe assembly file should end in the extension " << ASM_EXT <<
				"\nThe generated object's filename will end with " << OBJ_EXT <<
				endl;
		return 1;
	}

	// TODO Support both code and data segments
	// TODO Run through all lines, skipping comments
	// TODO Perform relocations
}

bool proc_line(unsigned num, const string &line) {
	vector<string> pieces;
	split(pieces, line, isspace);

	unsigned inst = 0;
	bool saw_opc = false;

	unsigned imms[9] = {};
	unsigned cur_opnd = 0;
	unsigned cur_imm = 0;

	for(string &piece : pieces) {
		string::size_type search_ind;

		// Label declaration
		if((search_ind = piece.find(':')) != piece.npos) {
			if(saw_opc)
				return error(num, "Label cannot be declared mid-line");
			if(search_ind != piece.size() - 1)
				return error(num, "Trailing characters after label");
			if(isdigit(piece[0]))
				return error(num, "Label names mustn't start with digits");

			// Trim off the colon
			string label = piece.substr(0, search_ind);

			if(symb_tab.count(label))
				return error(num, "Symbol already defined");
			else
				// Reference current PC position
				symb_tab[label] = cs.size();
		} // Instruction
		else if(!saw_opc) {
			transform(piece.begin(), piece.end(), piece.begin(), tolower);

			if(!OPC.count(piece))
				return error(num, "Unrecognized instruction mnemonic");
			else
				// Translate to opcode
				inst |= OPC.at(piece) << OPC_SHIFT;

			saw_opc = true;
		} // Address mode
		else {
			switch(piece[0]) {
				// Address of some sort
				case '%':
					// Scaled or doubly scaled displacement
					if(piece.find('(') != piece.npos) {
						// TODO Implement these modes
					}
					else {
						// PC-relative address
						if(piece[piece.length() - 1] == '@') {
							inst |= 02 << ADM_SHIFT[cur_opnd];

							try {
								imms[cur_imm] = stoi(piece.substr(1, piece.length() - 2));
							}
							catch(exception &ex) {
								return error(num, "Invalid PC-relative address");
							}

						}
						// Immediate address, which must reference a label
						else {
							inst |= 01 << ADM_SHIFT[cur_opnd];

							if(isdigit(piece[0]))
								return error(num, "Immediate address references label beginning with digit");

							// Reference current PC, plus one byte for the instruction word and each immediate word
							rel_tab[piece.substr(1)].push_back(cs.size() + 1 + cur_imm);
						}

						// We used up one immediate word
						++cur_imm;
					}
					break;
				// Something to do with a register
				case '$':
					inst |= 03 << ADM_SHIFT[cur_opnd];

					// Scaled or doubly scaled addressing
					if(piece.find('(') != piece.npos) {
						// TODO Implement these modes
					}
					else {
						char granular = piece[piece.length() - 1];
						string reg = piece.substr(1, piece.length() - 2);

						// Register indirect addressing
						if(granular == '%')
							imms[cur_imm] |= 01 << SADM_SHIFT;
						// Autoincrement or autodecrement
						else if(granular == '+' || granular == '-')
							imms[cur_imm] |= (granular == '+' ? 04 : 05) << SADM_SHIFT;
						// Register value
						else {
							imms[cur_imm] |= 00 << SADM_SHIFT;
							reg = piece.substr(1);
						}

						if(REG.count(reg))
							imms[cur_imm] |= REG.at(reg) << REG_SHIFT[0];
						else
							return error(num, "Invalid register name");

						// We used up one immediate word
						++cur_imm;
					}
					break;
				// An immediate, plain and simple
				default:
					if(!cur_opnd)
						return error(num, "First operand cannot be an immediate value");

					// Set flag to override the relevant address mode specifier
					inst |= 1 << IMM_SHIFT[cur_opnd - 1];
					break;
			}

			++cur_opnd;
		}
	}

	// TODO Shuttle inst and those imms that we actually used onto the end of cs

	return true;
}

bool error(unsigned line_num, const char *expl) {
	cerr << "error: line " << line_num << ": " << expl << endl;
	return false;
}
