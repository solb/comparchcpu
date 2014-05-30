/*
 * Copyright (C) 2014 Sol Boucher and Evan Klei
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with it.  If not, see <http://www.gnu.org/licenses/>.
 */

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
