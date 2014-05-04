// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Microcontroller hardware logic for decoding microprogram

#include "microcontroller.h"
#include "accessors.h"
#include "control_path.h"
#include "data_path.h"
#include "FlipRegister.h"
#include <Bus.h>
#include <BusALU.h>
#include <Clock.h>
#include <Counter.h>
#include <iostream>
#include <Memory.h>
#include <ShiftRegister.h>
#include <StorageObject.h>

using std::cerr;
using std::endl;

// Set the group of control points necessary to effect a line of RTL
static void execute_rtl(unsigned control_points);

// Is the conditional described by the current microinstruction satisfied?
// Precondition: We must currently be looking at a J-type microinstruction!
static bool decide_conditional();

// Given the register containing a jump table index, call that microprocedure
// Precondition: jt_index must be connected to uJBUS (i.e. it's uIR or regshift)
static void uprocedure_call(StorageObject &jt_index);

void interpret_microprogram() {
	bool proceed = true;
	while(proceed) {
		// Fetch the next microinstruction
		uabus.IN().pullFrom(upc);
		umem.MAR().latchFrom(uabus.OUT());
		Clock::tick();
		uir.latchFrom(umem.READ());
		umem.read();
		upc.incr();
		Clock::tick();

		// See hardware specification, section 3.2, first table
		switch(uinst_type()) {
			case 0x0: // C-type
				// Set the control points to make that happen in the datapath
				execute_rtl(uinst_c_ctrl());
				break;

			case 0xa: // Branch -> uJumpTab
				if(decide_conditional())
					break;
			case 0x1: // Jump -> uJumpTab
				// Replace the uPC with the value mapped to the jump table index
				ujbus.IN().pullFrom(uir);
				ujumptab.MAR().latchFrom(ujbus.OUT());
				Clock::tick();
				upc.latchFrom(ujumptab.READ());
				ujumptab.read();
				Clock::tick();
				break;

			case 0xb: // Branch -> ufetch
				if(decide_conditional())
					break;
			case 0x2: // Jump -> ufetch
				// Reset the uPC to point at the start of the program
				upc.clear();
				Clock::tick();
				break;

			case 0xc: // Call? <- uJumpTab
				if(decide_conditional())
					break;
			case 0x3: // Call <- uJumpTab
				// Save the current uPC to the uStack, then clobber it from jump
				uprocedure_call(uir);
				break;

			case 0xd: // Call? <- IR(opc)
				if(decide_conditional())
					break;
			case 0x4: // Call <- IR(opc)
				// Retrieve the opcode bits from the current instruction word
				dbus.IN().pullFrom(ir);
				regshift.latchFrom(dbus.OUT());
				Clock::tick();
				for(int count = 0; count < 8; ++count) {
					regshift.rightShift();
					Clock::tick();
				}

				// Save the current uPC to the uStack, then clobber it from jump
				uprocedure_call(regshift);
				break;

			case 0xe: // Return?
				if(decide_conditional())
					break;
			case 0x5: // Return
				// Restore the uPC from just under the micro--stack pointer
				usp.decr();
				Clock::tick();
				if(curr_ura() >= CTRL_STACK_HEIGHT)
					emergency_halt("interpret_microprogram()",
							"underflowed the microstack");
				uabus.IN().pullFrom(*ustack[curr_ura()]);
				upc.latchFrom(uabus.OUT());
				Clock::tick();
				break;

			case 0x7: // Halt abnormally
				cerr << "HALTING: Invalid operand combination" << endl;
			case 0x6: // Halt normally
				proceed = false;
				break;

			default: // Invalid
				cerr << "HALTING: Invalid microinstruction type: " <<
						uinst_type() << endl;
				proceed = false;
		}
	}
}

static void execute_rtl(unsigned control_points) {
	// See hardware specification, section 3.1 table
	// TODO Populate these cases
	switch(control_points) {
		case 0x00:
            alu.OP1().pullFrom(*addr[curr_opnd()]);
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_add);
            addr[curr_opnd()] -> latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x01:
            alu.OP1().pullFrom(*addr[curr_opnd()]);
            alu.OP2().pullFrom(pc);
            alu.perform(BusALU::op_add);
            addr[curr_opnd()] -> latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x02:
            alu.OP1().pullFrom(*addr[curr_opnd()]);
            alu.OP2().pullFrom(*reg[imm_r_reg(
                    *addr[curr_opnd()], 0)]);
            alu.perform(BusALU::op_add);
            addr[curr_opnd()] -> latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x03:
            alu.OP1().pullFrom(mdr);
            alu.perform(BusALU::op_rop1);
            addr[curr_opnd()] -> latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x04:
            abus.IN().pullFrom(pc);
            mem.MAR().latchFrom(abus.OUT());
            Clock::tick();
            mem.read();
            addr[curr_opnd()] -> latchFrom(mem.READ());
            Clock::tick();
			break;

		case 0x05:
            alu.OP1().pullFrom(*reg[imm_r_reg(mdr, 0)]);
            alu.perform(BusALU::op_rop1);
            addr[curr_opnd()] -> latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x06:
            alu.OP1().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 0)]);
            alu.perform(BusALU::op_rop1);
            addr[curr_opnd()] -> latchFrom( alu.OUT() );
            Clock::tick();
			break;

		case 0x07:
            alu.OP2().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 0)]);
            alu.OP1().pullFrom(*addr[curr_opnd()]);
            alu.perform(BusALU::op_add);
            addr[curr_opnd()] -> latchFrom( alu.OUT() );
            Clock::tick();
			break;

		case 0x08:
            alu.OP1().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 0)]);
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_lshift);
            addr[curr_opnd()] -> latchFrom( alu.OUT() );
            Clock::tick();
			break;

		case 0x09:
            alu.OP1().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 0)]);
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_lshift);
            addr[curr_opnd()] -> latchFrom( alu.OUT() );
            Clock::tick();
			break;

		case 0x0a:
            abus.IN().pullFrom(pc);
            mem.MAR().latchFrom(abus.OUT());
            Clock::tick();
            mem.read();
            ir.latchFrom(mem.READ());
            Clock::tick();
			break;

		case 0x0b:
            ir.flipBit(OPC_BRANCH_COMP_BIT);
            Clock::tick();
			break;

		case 0x0c:
            ir.flipBit(OPC_BRANCH_SIMP_BIT);
            Clock::tick();
			break;

		case 0x0d:
            mdr.latchFrom(alu.OUT());
            alu.perform(BusALU::op_zero);
            Clock::tick();
			break;

		case 0x0e:
            mdr.latchFrom(alu.OUT());
            alu.perform(BusALU::op_zero);
            Clock::tick();
            alu.OP1().pullFrom(mdr);
            alu.OP2().pullFrom(operand_n(1));
            mdr.latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x0f:
            mdr.latchFrom(alu.OUT());
            alu.perform(BusALU::op_one);
            Clock::tick();
			break;

		case 0x10:
            for(int count = 0; count < 8; ++count) {
			    mdr.rightShift();
				Clock::tick();
			}
			break;

		case 0x11:
            abus.IN().pullFrom(*addr[curr_opnd()]);
            mem.MAR().latchFrom(abus.OUT());
            Clock::tick();
            mem.read();
            mdr.latchFrom(mem.READ());
            Clock::tick();
			break;

		case 0x12:
            abus.IN().pullFrom(pc);
            mem.MAR().latchFrom(abus.OUT());
            Clock::tick();
            mem.read();
            mdr.latchFrom(mem.READ());
            Clock::tick();
			break;

		case 0x13:
            alu.OP1().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 1)]);
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_lshift);
            mdr.latchFrom(alu.OUT());
            Clock::tick();			
            break;

		case 0x14:
            alu.OP1().pullFrom(*reg[imm_r_reg(
                    *val[curr_opnd()], 2)]);
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_lshift);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x15:
            alu.OP1().pullFrom(tmp);
            alu.OP2().pullFrom(BUS_LOW_BYTE_MASK);
            alu.perform(BusALU::op_and);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x16:
            alu.perform(BusALU::op_one);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
            alu.OP1().pullFrom(operand_n(0));
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_add);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x17:
            alu.perform(BusALU::op_one);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
            alu.OP1().pullFrom(operand_n(0));
            alu.OP2().pullFrom(mdr);
            alu.perform(BusALU::op_sub);
            mdr.latchFrom(alu.OUT());
            Clock::tick();
			break;

		case 0x18:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.perform(BusALU::op_rop1);
            Clock::tick();
			break;

		case 0x19:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_and);
            Clock::tick();
			break;

		case 0x1a:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_add);
            Clock::tick();
			break;

		case 0x1b:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_sub);
            Clock::tick();
			break;

		case 0x1c:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_lshift);
            Clock::tick();
			break;

		case 0x1d:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_rshift);
            Clock::tick();
			break;

		case 0x1e:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_rashift);
            Clock::tick();
			break;

		case 0x1f:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_xor);
            Clock::tick();
			break;

		case 0x20:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.OP2().pullFrom(operand_n(2));
            alu.perform(BusALU::op_or);
			break;

		case 0x21:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullfrom(operand_n(2));
            alu.OP2().pullFrom(operand_n(1));
            alu.perform(BusALU::op_sub);
			break;

		case 0x22:
            mdr.latchFrom(alu.OUT());
            alu.OP1().pullFrom(operand_n(1));
            alu.perform(busALU::op_not);
            Clock::tick();
			break;

		case 0x23:
			break;

		case 0x24:
			break;

		case 0x25:
			break;

		case 0x26:
			break;

		case 0x27:
			alu.perform(BusALU::op_one);
			mdr.latchFrom(alu.OUT());
			Clock::tick();
			alu.OP1().pullFrom(*reg[14]);
			alu.OP2().pullFrom(mdr);
			reg[14]->latchFrom(alu.OUT());
			alu.perform(BusALU::op_add);
			Clock::tick();
			break;

		case 0x28:
			alu.perform(BusALU::op_one);
			mdr.latchFrom(alu.OUT());
			Clock::tick();
			alu.OP1().pullFrom(*reg[14]);
			alu.OP2().pullFrom(mdr);
			reg[14]->latchFrom(alu.OUT());
			alu.perform(BusALU::op_sub);
			Clock::tick();
			break;

		case 0x29:
			alu.OP2().pullFrom(pc);
			reg[15]->latchFrom(alu.OUT());
			alu.perform(BusALU::op_rop2);
			Clock::tick();
			break;

		case 0x2a:
			alu.perform(BusALU::op_one);
			mdr.latchFrom(alu.OUT());
			Clock::tick();
			alu.OP1().pullFrom(*reg[imm_r_reg(*val[curr_opnd()], 0)]);
			alu.OP2().pullFrom(mdr);
			reg[imm_r_reg(*val[curr_opnd()], 0)]->latchFrom(alu.OUT());
			alu.perform(BusALU::op_add);
			Clock::tick();
			break;

		case 0x2b:
			// TODO Is it okay that we're clobbering MDR here?
			alu.perform(BusALU::op_one);
			mdr.latchFrom(alu.OUT());
			Clock::tick();
			alu.OP1().pullFrom(*reg[imm_r_reg(*val[curr_opnd()], 0)]);
			alu.OP2().pullFrom(mdr);
			reg[imm_r_reg(*val[curr_opnd()], 0)]->latchFrom(alu.OUT());
			alu.perform(BusALU::op_sub);
			Clock::tick();
			break;

		case 0x2c:
			dbus.IN().pullFrom(mdr);
			reg[cntl_regid(0)]->latchFrom(dbus.OUT());
			Clock::tick();
			break;

		case 0x2d:
			dbus.IN().pullFrom(mdr);
			tmp.latchFrom(dbus.OUT());
			Clock::tick();
			break;

		case 0x2e:
			dbus.IN().pullFrom(mdr);
			val[1]->latchFrom(dbus.OUT());
			Clock::tick();
			break;

		case 0x2f:
			abus.IN().pullFrom(*addr[1]);
			mem.MAR().latchFrom(abus.OUT());
			Clock::tick();
			val[1]->latchFrom(mem.READ());
			mem.read();
			Clock::tick();
			break;

		case 0x30:
			abus.IN().pullFrom(*addr[2]);
			mem.MAR().latchFrom(abus.OUT());
			Clock::tick();
			val[2]->latchFrom(mem.READ());
			mem.read();
			Clock::tick();
			break;

		case 0x31:
			dbus.IN().pullFrom(mdr);
			val[curr_opnd()]->latchFrom(dbus.OUT());
			Clock::tick();
			break;

		case 0x32:
			abus.IN().pullFrom(pc);
			mem.MAR().latchFrom(abus.OUT());
			Clock::tick();
			val[curr_opnd()]->latchFrom(mem.READ());
			mem.read();
			Clock::tick();
			break;

		case 0x33:
			cntl[0]->clear();
			Clock::tick();
			cntl[0]->flipBit(3);
			Clock::tick();
			cntl[0]->flipBit(2);
			Clock::tick();
			cntl[0]->flipBit(1);
			Clock::tick();
			cntl[0]->flipBit(0);
			Clock::tick();
			break;

		case 0x34:
			cntl[0]->clear();
			Clock::tick();
			cntl[0]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			cntl[0]->flipBit(1);
			Clock::tick();
			cntl[0]->flipBit(0);
			Clock::tick();
			break;

		case 0x35:
			cntl[1]->clear();
			Clock::tick();
			cntl[1]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			cntl[1]->flipBit(0);
			Clock::tick();
			break;

		case 0x36:
			cntl[1]->clear();
			Clock::tick();
			cntl[1]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			cntl[1]->flipBit(1);
			Clock::tick();
			cntl[1]->flipBit(0);
			Clock::tick();
			break;

		case 0x37:
			pdbus.IN().pullFrom(*cntl[0]);
			cntl[1]->latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		case 0x38:
			pdbus.IN().pullFrom(utmp);
			cntl[1]->latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		case 0x39:
			cntl[1]->flipBit(CNTL_ISADDR_BIT);
			Clock::tick();
			break;

		case 0x3a:
			cntl[2]->clear();
			Clock::tick();
			cntl[2]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			break;

		case 0x3b:
			cntl[2]->clear();
			Clock::tick();
			cntl[2]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			cntl[2]->flipBit(1);
			Clock::tick();
			cntl[2]->flipBit(0);
			Clock::tick();
			break;

		case 0x3c:
			pdbus.IN().pullFrom(*cntl[1]);
			cntl[2]->latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		case 0x3d:
			cntl[2]->flipBit(CNTL_ISADDR_BIT);
			Clock::tick();
			break;

		case 0x3e:
			cntl[curr_opnd()]->clear();
			Clock::tick();
			pbus.IN().pullFrom(regshift);
			cntl[curr_opnd()]->latchFrom(pbus.OUT());
			Clock::tick();
			break;

		case 0x3f:
			cntl[curr_opnd()]->clear();
			Clock::tick();
			cntl[curr_opnd()]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			break;

		case 0x40:
			cntl[curr_opnd()]->clear();
			Clock::tick();
			cntl[curr_opnd()]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			pdbus.IN().pullFrom(i);
			cntl[curr_opnd()]->latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		case 0x41:
			cntl[curr_opnd()]->clear();
			Clock::tick();
			cntl[curr_opnd()]->flipBit(CNTL_ISNTGPR_BIT);
			Clock::tick();
			cntl[curr_opnd()]->flipBit(CNTL_ISADDR_BIT);
			Clock::tick();
			// TODO We need to make sure that this only modifies the lowest bits
			pdbus.IN().pullFrom(i);
			cntl[curr_opnd()]->latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		case 0x42:
			i.clear();
			Clock::tick();
			break;

		case 0x43:
			i.incr();
			Clock::tick();
			break;

		case 0x44:
			dbus.IN().pullFrom(mdr);
			regshift.latchFrom(dbus.OUT());
			Clock::tick();
			break;

		case 0x45:
			for(int count = 0; count < 8; ++count) {
				regshift.rightShift();
				Clock::tick();
			}
			break;

		case 0x46:
			pdbus.IN().pullFrom(*cntl[2]);
			utmp.latchFrom(pdbus.OUT());
			Clock::tick();
			break;

		default:
			emergency_halt("execute_rtl()",
					"Invalid microinstruction control point identifier");
	}
}

void uprocedure_call(StorageObject &jt_index) {
	// Save the current uPC to the uStack, then clobber it from jump table entry
	uabus.IN().pullFrom(upc);
	if(curr_ura() >= CTRL_STACK_HEIGHT)
		emergency_halt("uprocedure_call()",
				"overflowed the microstack");
	ustack[curr_ura()]->latchFrom(uabus.OUT());
	ujbus.IN().pullFrom(jt_index);
	ujumptab.MAR().latchFrom(ujbus.OUT());
	Clock::tick();
	usp.incr();
	upc.latchFrom(ujumptab.READ());
	ujumptab.read();
	Clock::tick();
}

bool decide_conditional() {
	// See hardware specification, section 3.2, second table
	switch(uinst_j_cond()) {
		case 0x00:
			return inst_ami() == 0;

		case 0x01:
			return inst_ami() == 1 || inst_ami() == 2;

		case 0x02:
			return inst_ami() == 2;

		case 0x03:
			return inst_ami() == 3;

		case 0x04:
			return !logic_res();

		case 0x05:
			return logic_res();

		case 0x06:
			return neg_res();

		case 0x07:
			return inst_sam() == 00;

		case 0x08:
			return inst_sam() == 01;

		case 0x09:
			return inst_sam() == 02;

		case 0x0a:
			return inst_sam() == 03;

		case 0x0b:
			return inst_sam() == 04;

		case 0x0c:
			return inst_sam() == 05;

		case 0x0d:
			return inst_sam() == 06;

		case 0x0e:
			return inst_sam() == 07;

		case 0x0f:
			return (*cntl[0])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x10:
			return !cntl_isntgpr(0);

		case 0x11:
			return cntl_isntgpr(0);

		case 0x12:
			return (*cntl[1])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x13:
			return cntl_isntgpr(1) && cntl_isaddr(1);

		case 0x14:
			return (*cntl[2])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x15:
			return cntl_isntgpr(2) && cntl_isaddr(2);

		case 0x16:
			return curr_opnd() != 0 && inst_immi();

		case 0x17:
			return curr_opnd() != 3;

		case 0x18:
			return operand_n(1)(WORD_SIZE - 1, 0) == 0;

		default:
			return emergency_halt("decide_conditional()",
					"Invalid microinstruction condition code");
	}
}
