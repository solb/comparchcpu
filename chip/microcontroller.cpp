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
				// TODO Implement table-based jump
				break;

			case 0xb: // Branch -> ufetch
				if(!decide_conditional())
					break;
			case 0x2: // Jump -> ufetch
				// TODO Implement jump-to-start
				break;

			case 0xc: // Call? <- uJumpTab
				if(!decide_conditional())
					break;
			case 0x3: // Call <- uJumpTab
				// Implement table-based call
				break;

			case 0xd: // Call? <- IR(opc)
				if(!decide_conditional())
					break;
			case 0x4: // Call <- IR(opc)
				// Implement opcode-based call
				break;

			case 0xe: // Return?
				if(!decide_conditional())
					break;
			case 0x5: // Return
				// Implement return
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
