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

// Constants
const unsigned WORD_SIZE = 16;
const size_t NUM_GPRS = 16;
const size_t NUM_OPNDS = 3;

// Construct buses
Bus abus("ABUS", WORD_SIZE);
Bus dbus("DBUS", WORD_SIZE);

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
	for(size_t index = 0; index < NUM_GPRS; ++index) {
		reg_internal[index] = new StorageObject("GPR", WORD_SIZE);
		if(index < NUM_OPNDS) {
			val_internal[index] = new StorageObject("VAL", WORD_SIZE);
			addr_internal[index] = new StorageObject("ADDR", WORD_SIZE);
		}
	}
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
