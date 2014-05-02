// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Convenience functions exposing key bits of certain registers

#ifndef ACCESSORS_H_
#define ACCESSORS_H_

#include <cstdlib>

class StorageObject;

// Current microinstruction's type field
unsigned uinst_type();

// Current (C-type) microinstruction's control point specifier
unsigned uinst_c_ctrl();

// Current (J-type) microinstruction's condition code
unsigned uinst_j_cond();

// Current (J-type) microinstruction's jump index
unsigned uinst_j_jump();

// Index of microstack register currently holding micro--return address
unsigned curr_ura();

// Current operand being processed
unsigned curr_opnd();

// Current instruction/operand combination's immediate override flag
bool inst_immi();

// Current instruction/operand combination's primary address mode
unsigned inst_ami();

// Current instruction/operand combination's secondary address mode
// Precondition: MDR must currently contain an R-type immediate word!
unsigned inst_sam();

// Get the register to operate on
// Precondition: srcReg contains an R-type immediate word. 
unsigned imm_r_reg(const StorageObject &srcReg, size_t operand);

// Verdict of the last logical ALU instruction
// Precondition: MDR must still contain the value computed by this operation!
bool logic_res();

// Immediately bail out of the entire system with a serious internal error
unsigned emergency_halt(const char *function, const char *excuse);

#endif
