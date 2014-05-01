// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Data path hardware component initializations

#include "data_path.h"
#include <Bus.h>
#include <BusALU.h>
#include <Counter.h>
#include <Memory.h>
#include <ShiftRegister.h>
#include <StorageObject.h>

// Construct buses
Bus abus("ABUS", WORD_SIZE);
Bus dbus("DBUS", WORD_SIZE);

// Construct constants
static StorageObject bus_low_byte_mask_internal("0xFF", WORD_SIZE, 0xff);
const StorageObject &BUS_LOW_BYTE_MASK = bus_low_byte_mask_internal;

// Construct registers
Counter pc("PC", WORD_SIZE);
ShiftRegister ir("IR", WORD_SIZE);
ShiftRegister mdr("MDR", WORD_SIZE);
StorageObject tmp("TMP", WORD_SIZE);

// Construct banks
static StorageObject *reg_internal[NUM_GPRS];
StorageObject *const *const reg = reg_internal;
static StorageObject *val_internal[NUM_OPNDS];
StorageObject *const *const val = val_internal;
static StorageObject *addr_internal[NUM_OPNDS];
StorageObject *const *const addr = addr_internal;
Memory mem("MEM", WORD_SIZE, WORD_SIZE);

// Construct workhorse
BusALU alu("ALU", WORD_SIZE);

void wire_data_path() {
	bus_low_byte_mask_internal.connectsTo(alu.OP2());

	pc.connectsTo(abus.IN());
	pc.connectsTo(abus.OUT());
	pc.connectsTo(dbus.OUT());
	pc.connectsTo(mem.READ());
	pc.connectsTo(alu.OP2());

	ir.connectsTo(mem.READ());

	mdr.connectsTo(dbus.IN());
	mdr.connectsTo(dbus.OUT());
	mdr.connectsTo(mem.READ());
	mdr.connectsTo(mem.WRITE());
	mdr.connectsTo(alu.OP1());
	mdr.connectsTo(alu.OP2());
	mdr.connectsTo(alu.OUT());

	tmp.connectsTo(dbus.OUT());
	tmp.connectsTo(alu.OP1());

	for(size_t index = 0; index < NUM_GPRS; ++index) {
		reg_internal[index] = new StorageObject("GPR", WORD_SIZE);
		reg[index]->connectsTo(abus.IN());
		reg[index]->connectsTo(dbus.IN());
		reg[index]->connectsTo(dbus.OUT());
		reg[index]->connectsTo(mem.WRITE());
		reg[index]->connectsTo(alu.OP1());
		reg[index]->connectsTo(alu.OP2());
		reg[index]->connectsTo(alu.OUT());

		if(index < NUM_OPNDS) {
			val_internal[index] = new StorageObject("VAL", WORD_SIZE);
			val[index]->connectsTo(dbus.IN());
			val[index]->connectsTo(dbus.OUT());
			val[index]->connectsTo(alu.OP1());
			val[index]->connectsTo(alu.OP2());
			val[index]->connectsTo(mem.READ());

			addr_internal[index] = new StorageObject("ADDR", WORD_SIZE);
			addr[index]->connectsTo(abus.IN());
			addr[index]->connectsTo(alu.OP1());
			addr[index]->connectsTo(alu.OUT());
			addr[index]->connectsTo(mem.READ());
		}
	}

	mem.MAR().connectsTo(abus.OUT());
}

void melt_data_path() {
	for(size_t index = 0; index < NUM_GPRS; ++index) {
		delete reg_internal[index];
		reg_internal[index] = NULL;
		if(index < NUM_OPNDS) {
			delete val_internal[index];
			val_internal[index] = NULL;
			delete addr_internal[index];
			addr_internal[index] = NULL;
		}
	}
}
