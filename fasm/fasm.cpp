// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: assembler

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
using boost::split;
using boost::trim;
using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::hex;
using std::ifstream;
using std::list;
using std::make_pair;
using std::min;
using std::ofstream;
using std::pair;
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
bool scaled_ams(unsigned, list<string> &, unsigned, unsigned &, unsigned &);
bool isparen(char);
template<typename E> E fruit_pop(list<E> &);

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "USAGE: " << argv[0] << " <assembly file>" <<
				"\nThe assembly file should end in the extension " << ASM_EXT <<
				"\nThe generated object's filename will end with " << OBJ_EXT <<
				endl;
		return 1;
	}

	// Try to open the source file
	ifstream src(argv[1]);
	if(!src.is_open()) {
		cerr << "Unable to open the specified source file" << endl;
		return 2;
	}

	bool noerrors = true;
	unsigned linenum = 1;
	string line;
	while(getline(src, line)) {
		string::size_type comment = line.find('#');
		if(comment != line.npos)
			line = line.substr(0, comment);

		trim(line);

		// TODO Support data segment, or at least allow leaving space for data
		if(line.size())
			noerrors = proc_line(linenum, line) && noerrors;

		++linenum;
	}
	src.close();

	// Perform relocations
	for(pair<const string, list<unsigned>> &reloc : rel_tab) {
		if(!symb_tab.count(reloc.first)) {
			cerr << "Undefined symbol: " << reloc.first << endl;
			return 4;
		}

		unsigned tgt = symb_tab[reloc.first];
		for(unsigned ref : reloc.second)
			cs[ref] = tgt;
	}

	if(!noerrors)
		return 3;

	// Decide what to name the output file
	string filename(argv[1]);
	string::size_type ext = filename.find_last_of(ASM_EXT);
	if(ext != filename.npos)
		filename = filename.substr(0, ext);
	filename += OBJ_EXT;

	ofstream bin(filename);
	bin << hex;
	for(unsigned addr = 0; addr < cs.size(); ++addr)
		bin << addr << ' ' << '1' << ' ' << cs[addr] << endl;

	// TODO Use a main label?
	bin << '0';
}

bool proc_line(unsigned num, const string &line) {
	vector<string> pieces;
	split(pieces, line, isspace);

	unsigned inst = 0;
	bool saw_opc = false;

	unsigned imms[9] = {};
	unsigned cur_opnd = 0;
	unsigned cur_imm = 0;

	if(pieces.size() > 4 || (pieces.size() == 4 && line.find(':') == line.npos))
		return error(num, "More than three operands to instruction");

	for(string &piece : pieces) {
		string::size_type search_ind;

		// Truncate at trailing comma if present
		if(piece[piece.length() - 1] == ',')
			piece = piece.substr(0, piece.length() - 1);

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
						list<string> terms;
						split(terms, piece, isparen);
						string base = fruit_pop(terms).substr(1);

						// It's a literal offset
						if(isdigit(base[0])) {
							try {
								// Copy the address into the binary, leaving space for our shifts
								imms[cur_imm + 2] = stoi(base);
							}
							catch(exception &ex) {
								return error(num, "Invalid literal displacement base address");
							}
						}
						// It's a label
						else
							rel_tab[base].push_back(cs.size() + 1 + cur_imm);

						// Distinguish from plain scaled addressing
						imms[cur_imm] |= 01 << (SADM_SHIFT + 2);

						// Encode the scaling information
						scaled_ams(num, terms, 0, imms[cur_imm], imms[cur_imm + 1]);

						// We used up 3 immediate words
						cur_imm += 3;
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
						list<string> terms;
						split(terms, piece, isparen);
						string reg = fruit_pop(terms).substr(1);

						// Here's the displacement
						if(REG.count(reg))
							imms[cur_imm] |= REG.at(reg) << REG_SHIFT[0];
						else
							return error(num, "Invalid base register for scaled addressing");

						// Encode the scaling information
						scaled_ams(num, terms, 1, imms[cur_imm], imms[cur_imm + 1]);

						// We used up 2 immediate words
						cur_imm += 2;
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

	// Queue the instructions for writing into the code segment
	cs.push_back(inst);
	for(unsigned copied = 0; copied < cur_imm; ++copied)
		cs.push_back(imms[copied]);

	return true;
}

bool error(unsigned line_num, const char *expl) {
	cerr << "error: line " << line_num << ": " << expl << endl;
	return false;
}

bool scaled_ams(unsigned num, list<string> &terms, unsigned reg_offst, unsigned &s_imm, unsigned &i_imm) {
	if(terms.size() > 2)
		return error(num, "Scaled address modes accept at most 2 offsets");

	s_imm |= (terms.size() == 2 ? 011 : 010) << SADM_SHIFT;

	for(list<string>::size_type index = 0; index < terms.size(); ++index) {
		list<string> coeffs;
		string cur_term = fruit_pop(terms);
		split(coeffs, cur_term, [] (char symb) -> bool {return symb == ',' || isspace(symb);});

		if(coeffs.size() > 2)
			return error(num, "Scaled address mode terms may have at most 2 coefficients");

		string reg = fruit_pop(coeffs).substr(1);
		if(REG.count(reg))
			s_imm |= REG.at(reg) << REG_SHIFT[reg_offst + index];
		else
			return error(num, "Invalid register name in scaled address specifier");

		if(coeffs.size()) {
			try {
				i_imm |= stoi(fruit_pop(coeffs)) << SIMM_SHIFT[index];
			}
			catch(exception &ex) {
				return error(num, "Non-integral shift in scaled address specifier");
			}
		}
	}

	return true;
}

bool isparen(char symb) {
	return symb == '(' || symb == ')';
}

template<typename E> E fruit_pop(list<E> &cont) {
	E res = cont.front();
	cont.pop_front();
	return res;
}
