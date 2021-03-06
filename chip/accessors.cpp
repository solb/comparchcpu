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

// Implementation of functions exposing key bits of certain registers

#include "accessors.h"
#include "control_path.h"
#include "data_path.h"
#include "FlipRegister.h"
#include <cstdlib>
#include <Counter.h>
#include <StorageObject.h>

const unsigned CNTL_ISNTGPR_BIT = CTRL_CNTL_WIDTH - 1;
const unsigned CNTL_ISADDR_BIT = 2;

const unsigned OPC_BRANCH_COMP_LOGICAL_BIT = 11;
const unsigned OPC_BRANCH_SIMP_BIT = 10;

unsigned uinst_type() {
	// Hardware specification, section 3
	return uir(CTRL_INST_WIDTH - 1, 12);
}

unsigned uinst_c_ctrl() {
	// Hardware specification, section 3, first table
	return uir(11, 0);
}

unsigned uinst_j_cond() {
	// Hardware specification, section 3, second table
	return uir(11, 7);
}

unsigned uinst_j_jump() {
	// Hardware specification, section 3, second table
	return uir(6, 0);
}

unsigned curr_ura() {
	return usp.value();
}

unsigned curr_opnd() {
	return i.value();
}

bool inst_immi() {
	// ISA reference, section 2.1, first table
	switch(curr_opnd()) {
		case 1:
			return ir(7);

		case 2:
			return ir(6);

		default:
			return emergency_halt("inst_immi()",
					"invalid contents of i register");
	}
}

unsigned inst_ami() {
	// ISA reference, section 2.1, first table
	switch(curr_opnd()) {
		case 0:
			return ir(5, 4);

		case 1:
			return ir(3, 2);

		case 2:
			return ir(1, 0);

		default:
			return emergency_halt("inst_ami()",
					"invalid contents of i register");
	}
}

unsigned inst_sam() {
	// ISA reference, section 2.2, third table
	return mdr(WORD_SIZE - 1, 12);
}

unsigned imm_r_reg(const StorageObject &srcReg, size_t operand) {
    // ISA reference, section 2.2, third table
    switch(operand) {
        case 0:
            return srcReg(11, 8);

        case 1:
            return srcReg(7, 4);

        case 2:
            return srcReg(3, 0);

        default:
            return emergency_halt("imm_r_reg()","invalid operand given");
    }
}

StorageObject &operand_n(size_t operandId) {
    if(operandId != 0 && operandId != 1 && operandId != 2)
        emergency_halt("operand_n()", "invalid operand given");

    if(!cntl_isntgpr(operandId)) {
        return *reg[cntl_regid(operandId)];
    } else if(!cntl_isaddr(operandId)) {
        if(cntl_valoraddr(operandId) == 3) {
            return mdr;
        } else {
            return *val[cntl_valoraddr(operandId)];
        }
    } else {
        if(cntl_valoraddr(operandId) == 3) {
            emergency_halt("operand_n()", 
                    "cannot use MDR as an address");
        }
        return *addr[cntl_valoraddr(operandId)];
    }
}

bool cntl_isntgpr(size_t operandId) {
    return (*cntl[operandId])(CNTL_ISNTGPR_BIT);
}

bool cntl_isaddr(size_t operandId) {
    if(!cntl_isntgpr(operandId)) {
        emergency_halt("cntl_isaddr()", "operand is a register type");
    }
    return (*cntl[operandId])(CNTL_ISADDR_BIT);
}

size_t cntl_regid(size_t operandId) {
    if(cntl_isntgpr(operandId)) {
        emergency_halt("cntl_regid()", "operand is not a register type");
    }
    return (*cntl[operandId])(3, 0);
}

size_t cntl_valoraddr(size_t operandId) {
    if(!cntl_isntgpr(operandId)) {
        emergency_halt("cntl_valoraddr()", "operand is a register type");
    }
    return (*cntl[operandId])(1, 0);
}

bool logic_res() {
	return mdr.value() == 1;
}

bool logic_nres() {
	return mdr.value() == 0;
}

bool neg_res() {
	return mdr(WORD_SIZE - 1);
}

unsigned emergency_halt(const char *function, const char *excuse) {
	cerr << endl << "FATAL: " << function << ' ' << excuse << endl;
	exit(3);
	return -1;
}
