// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Control path hardware components

#ifndef CONTROL_PATH_H_
#define CONTROL_PATH_H_

#include <cstddef>

// Forward declarations
class Bus;
class Counter;
class FlipRegister;
class Memory;
class ShiftRegister;
class StorageObject;

// Width of microinstruction words
extern const unsigned CTRL_INST_WIDTH;

// Width of microprogram memory addresses
extern const unsigned CTRL_ADDR_WIDTH;

// Width of jump table indices
extern const unsigned CTRL_JADR_WIDTH;

// Width of register identifiers
extern const unsigned CTRL_RADR_WIDTH;

// Width of control registers
extern const unsigned CTRL_CNTL_WIDTH;

// Width of control register selector
extern const unsigned CTRL_OPDI_WIDTH;

// Height of the stack
extern const size_t CTRL_STACK_HEIGHT;

// Width of stack pointer
extern const unsigned CTRL_SPTR_WIDTH;

// Buses
extern Bus uabus; // ADDR_WIDTH wide
extern Bus ujbus; // JADR_WIDTH wide
extern Bus pbus; // RADR_WIDTH wide
extern Bus pdbus; // CNTL_WIDTH wide

// Registers
extern Counter upc; // ADDR_WIDTH wide
extern StorageObject uir; // INST_WIDTH wide
extern StorageObject utmp; // CNTL_WIDTH wide
extern Counter i; // OPDI_WIDTH wide
extern ShiftRegister regshift; // INST_WIDTH wide
extern Counter usp; // SPTR_WIDTH wide

// Banks of storage
extern FlipRegister *const *const cntl; // CNTL_WIDTH wide
extern StorageObject *const *const ustack; // ADDR_WIDTH wide
extern Memory umem; // INST_WIDTH data, ADDR_WIDTH address
extern Memory ujumptab; // INST_WIDTH data, CTRL_JADR_WIDTH address

// Connect the control path components
void wire_control_path();

// Load the microcode and its jump table and initialize the uPC
void load_microprogram(const char *ucode, const char *ulabels);

// Release internal dynamic memory
void melt_control_path();

#endif
