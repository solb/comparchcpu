// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Data path hardware components

#ifndef DATA_PATH_H_
#define DATA_PATH_H_

#include <cstddef>

// Word size
extern const unsigned WORD_SIZE;

// Number of general purpose registers
extern const size_t NUM_GPRS;

// Number of operands (and hence supporting components)
extern const size_t NUM_OPNDS;

// Forward declarations
class Bus;
class BusALU;
class Counter;
class Memory;
class ShiftRegister;
class StorageObject;

// Buses
extern Bus abus;
extern Bus dbus;

// Constants
extern const StorageObject &BUS_LOW_BYTE_MASK;

// Registers
extern Counter pc;
extern ShiftRegister ir; // TODO Should be a flippable shifter
extern ShiftRegister mdr;
extern StorageObject tmp;

// Banks of storage
extern StorageObject *const *const reg;
extern StorageObject *const *const val;
extern StorageObject *const *const addr;
extern Memory mem;

// The workhorse
extern BusALU alu;

// Connect the data path components
void wire_data_path();

// Release internal dynamic memory
void melt_data_path();

#endif
