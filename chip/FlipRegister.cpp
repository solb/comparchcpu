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

// implementation of the FlipRegister

#include <cstdlib>
#include <iostream>

#include <StorageObject.h>
#include "FlipRegister.h"

using namespace std;

FlipRegister::FlipRegister (const char *id, int numBits, long initVal):
    CPUObject( id, numBits ),
    StorageObject( id, numBits, initVal ),
    ShiftRegister(id, numBits, initVal),
    bitsToFlip( -1 ),
    flipBits( false ){
}

FlipRegister::~FlipRegister(){
}

void FlipRegister::flipBit( int bit ){
    bitsToFlip = bit;
    flipBits = true;
}
void FlipRegister::perform (ShiftRegister::Operation op){
    flipBits = false;
    ShiftRegister::perform ( op );
}
void FlipRegister::clear(){
    flipBits = true;
    bitsToFlip = -1;
}
void FlipRegister::phase1(){
    if(flipBits && bitsToFlip >= 0){
        int mask = 1;
        int i = bitsToFlip;
        while ( i > 0 ){
            mask = mask << 1;
            i --;
        }
        if( i < 0 ){
            mask = 0;
        }
        value(value()^mask);      
    }else if(flipBits && bitsToFlip < 0){
        value(0);
    }else{
        ShiftRegister::phase1();
    }
    flipBits = false;
    bitsToFlip = -1;
}
