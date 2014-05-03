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
	return usp(CTRL_SPTR_WIDTH - 1, 0);
}

unsigned curr_opnd() {
	return i(CTRL_OPDI_WIDTH - 1, 0);
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
    if(operandId > 3) { // TODO Are we allowed to do this
        emergency_halt("operand_n()", "invalid operand given");
    }
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
    return (*cntl[operandId])(CTRL_CNTL_WIDTH - 1);
}

bool cntl_isaddr(size_t operandId) {
    if(!cntl_isntgpr(operandId)) {
        emergency_halt("cntl_isaddr()", "operand is a register type");
    }
    return (*cntl[operandId])(2);
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
	return mdr(WORD_SIZE - 1, 0) == 1;
}

bool neg_res() {
	return mdr(WORD_SIZE - 1);
}

unsigned emergency_halt(const char *function, const char *excuse) {
	cerr << "FATAL: " << function << excuse << endl;
	exit(3);
	return -1;
}
