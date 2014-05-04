// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Convenience functions exposing key bits of certain registers

#ifndef ACCESSORS_H_
#define ACCESSORS_H_

#include <cstdlib>

class StorageObject;

// Determines whether an operand is a GPR index
extern const unsigned CNTL_ISNTGPR_BIT;

// Determines whether an operand is an address rather than a value
extern const unsigned CNTL_ISADDR_BIT;

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

// Gets the register containing the nth operand
StorageObject &operand_n(size_t operandId);

// Determines whether the nth operand is a GPR index
bool cntl_isntgpr(size_t operandId);

// Determines whether the nth operand is an address rather than a value
// Precondition: nth operand is not a GPR
bool cntl_isaddr(size_t operandId);

// Gets the register id stored in the nth operand 
// Precondition: nth operand is a GPR
size_t cntl_regid(size_t operandId);

// Gets the val or addr index from the nth operand
// Precondition: nth operand is not a GPR
size_t cntl_valoraddr(size_t operandId);

// Verdict of the last logical ALU instruction
// Precondition: MDR must still contain the value computed by this operation!
bool logic_res();

// Whether the last arithmetic ALU instruction yielded a negative value
// Precondition: MDR must still containt the value computed by that operation!
bool neg_res();

// Immediately bail out of the entire system with a serious internal error
unsigned emergency_halt(const char *function, const char *excuse);

#endif
