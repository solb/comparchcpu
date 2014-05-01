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
				// TODO Implement control point setting
				break;

			case 0xa: // Branch -> uJumpTab
				if(!decide_conditional())
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
				if(!decide_conditional())
					break;
			case 0x2: // Jump -> ufetch
				// Reset the uPC to point at the start of the program
				upc.clear();
				Clock::tick();
				break;

			case 0xc: // Call? <- uJumpTab
				if(!decide_conditional())
					break;
			case 0x3: // Call <- uJumpTab
				// Save the current uPC to the uStack, then clobber it from jump
				uprocedure_call(uir);
				break;

			case 0xd: // Call? <- IR(opc)
				if(!decide_conditional())
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
				if(!decide_conditional())
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
			return logic_res();

		case 0x05:
			return inst_sam() == 00;

		case 0x06:
			return inst_sam() == 01;

		case 0x07:
			return inst_sam() == 02;

		case 0x08:
			return inst_sam() == 03;

		case 0x09:
			return inst_sam() == 04;

		case 0x0a:
			return inst_sam() == 05;

		case 0x0b:
			return inst_sam() == 06;

		case 0x0c:
			return inst_sam() == 07;

		case 0x0d:
			return (*cntl[0])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x0e:
			return (*cntl[0])(4) == 0;

		case 0x0f:
			return (*cntl[0])(4) == 1;

		case 0x10:
			return (*cntl[1])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x11:
			return (*cntl[1])(4) == 1 && (*cntl[1])(2) == 1;

		case 0x12:
			return (*cntl[2])(CTRL_CNTL_WIDTH - 1, 0) == 0x10;

		case 0x13:
			return (*cntl[2])(4) == 1 && (*cntl[2])(4) == 1;

		case 0x14:
			return curr_opnd() != 0 && inst_immi();

		case 0x15:
			return curr_opnd() != 3;

		default:
			return emergency_halt("decide_conditional()",
					"Invalid microinstruction condition code");
	}
}