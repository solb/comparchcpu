/*
 * Copyright (C) 2014 Sol Boucher and Evan Klei
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with it.  If not, see <http://www.gnu.org/licenses/>.
 */

// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Data path hardware components

#ifndef DATA_PATH_H_
#define DATA_PATH_H_

#include <cstddef>

// Word size
const unsigned WORD_SIZE = 16;

// Number of general purpose registers
const size_t NUM_GPRS = 16;

// Number of operands (and hence supporting components)
const size_t NUM_OPNDS = 3;

// Forward declarations
class Bus;
class BusALU;
class Counter;
class FlipRegister;
class Memory;
class ShiftRegister;
class StorageObject;

// Buses
extern Bus abus;
extern Bus dbus;

// Constants
// TODO Fix const correctness bug in arch lib
extern StorageObject &BUS_LOW_BYTE_MASK;

// Registers
extern Counter pc;
extern FlipRegister ir;
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

// Load the program and initialize the PC
void load_userprogram(const char *objfile);

// Release internal dynamic memory
void melt_data_path();

#endif
