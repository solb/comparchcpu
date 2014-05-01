// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Control path hardware component initializations

#include "control_path.h"
#include "data_path.h"
#include "FlipRegister.h"
#include <Bus.h>
#include <Clock.h>
#include <Counter.h>
#include <Memory.h>
#include <ShiftRegister.h>
#include <StorageObject.h>

// Constants
const unsigned CTRL_INST_WIDTH = 16;
const unsigned CTRL_ADDR_WIDTH = 12;
const unsigned CTRL_JADR_WIDTH = 7;
const unsigned CTRL_RADR_WIDTH = 4; // Must be large enough to address NUM_GPRS
const unsigned CTRL_CNTL_WIDTH = 5;
const unsigned CTRL_OPDI_WIDTH = 2; // Must be big enough to address NUM_OPNDS
const size_t CTRL_STACK_HEIGHT = 3;
const unsigned CTRL_SPTR_WIDTH = 2; // Must be be able to address STACK_HEIGHT

// Construct buses
Bus uabus("uABUS", CTRL_ADDR_WIDTH);
Bus ujbus("uJBUS", CTRL_JADR_WIDTH);
Bus pbus("pBUS", CTRL_RADR_WIDTH);
Bus pdbus("pDBUS", CTRL_CNTL_WIDTH);

// Construct registers
Counter upc("uPC", CTRL_ADDR_WIDTH);
StorageObject uir("uIR", CTRL_INST_WIDTH);
StorageObject utmp("uTMP", CTRL_CNTL_WIDTH);
Counter i("i", CTRL_OPDI_WIDTH);
ShiftRegister regshift("regshift", WORD_SIZE);
Counter usp("uSP", CTRL_SPTR_WIDTH);

// Construct banks
static FlipRegister *cntl_internal[NUM_OPNDS];
FlipRegister *const *const cntl = cntl_internal;
static StorageObject *ustack_internal[CTRL_STACK_HEIGHT];
StorageObject *const *const ustack = ustack_internal;
Memory umem("uMEM", CTRL_ADDR_WIDTH, CTRL_INST_WIDTH);
Memory ujumptab("uJumpTab", CTRL_JADR_WIDTH, CTRL_ADDR_WIDTH);

void wire_control_path() {
	upc.connectsTo(uabus.IN());
	upc.connectsTo(uabus.OUT());
	upc.connectsTo(umem.READ());
	upc.connectsTo(ujumptab.READ());

	uir.connectsTo(ujbus.IN());
	uir.connectsTo(umem.READ());

	utmp.connectsTo(pdbus.IN());
	utmp.connectsTo(pdbus.OUT());

	i.connectsTo(pdbus.IN());

	regshift.connectsTo(ujbus.IN());
	regshift.connectsTo(pbus.IN());
	regshift.connectsTo(abus.OUT());
	regshift.connectsTo(dbus.OUT());

	for(size_t index = 0; index < CTRL_STACK_HEIGHT; ++index) {
		if(index < NUM_OPNDS) {
			cntl_internal[index] = new FlipRegister("cntl", CTRL_CNTL_WIDTH);
			cntl[index]->connectsTo(pbus.OUT());
			cntl[index]->connectsTo(pdbus.IN());
			cntl[index]->connectsTo(pdbus.OUT());
		}

		ustack_internal[index] = new StorageObject("uStack", CTRL_ADDR_WIDTH);
		ustack[index]->connectsTo(uabus.IN());
		ustack[index]->connectsTo(uabus.OUT());
	}

	umem.MAR().connectsTo(uabus.OUT());

	ujumptab.MAR().connectsTo(ujbus.OUT());
}

void load_microprogram(const char *ucode, const char *ulabels) {
	umem.load(ucode);
	ujumptab.load(ulabels);
	upc.latchFrom(umem.READ());
	Clock::tick();
}

void melt_control_path() {
	for(size_t index = 0; index < CTRL_STACK_HEIGHT; ++index) {
		if(index < NUM_OPNDS) {
			delete cntl_internal[index];
			cntl_internal[index] = NULL;
		}
		delete ustack_internal[index];
		ustack_internal[index] = NULL;
	}
}
