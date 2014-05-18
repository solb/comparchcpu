// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// List of instruction mnemonics

#include "opcodes.h"

const size_t NUM_OPCS = 45;

const char *const OPC_MNEMS[] = {"HALT",

							 	 "AND",
							 	 "OR",
							 	 "XOR",
							 	 "LSFT",
							 	 "NAND",
							 	 "NOR",
							 	 "XNOR",
							 	 "RSFT",
							 	 "LAND",
							 	 "LOR",
							 	 "LXOR",
							 	 "RASFT",
							 	 "LNAND",
							 	 "LNOR",
							 	 "LXNOR",
							 	 "SLT",
							 	 "SGT",
							 	 "SEQ",
							 	 "SNE",
							 	 "SLE",
							 	 "SGE",
							 	 "ADD",
							 	 "SUB",

							 	 "BLT",
							 	 "BGT",
							 	 "BEQ",
							 	 "BNE",
							 	 "BLE",
							 	 "BGE",

							 	 "PRNT",

							 	 "LNOT",
							 	 "SIZ",
							 	 "SNZ",
							 	 "NOT",
							 	 "NEG",

							 	 "BIZ",
							 	 "BNZ",

							 	 "INCR",
							 	 "DECR",

							 	 "JMP",
							 	 "JAL",
							 	 "CALL",

							 	 "RET",

							 	 "MOV"};
