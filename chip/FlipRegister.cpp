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

void FlipRegister::flipBit( int bit){
    bitsToFlip = bit;
    flipBits = true;
}
void FlipRegister::perform (ShiftRegister::Operation op){
    flipBits = false;
    ShiftRegister::perform ( op );
}

void FlipRegister::phase1(){
    if(flipBits){
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
      
    }else{
        ShiftRegister::phase1();
    }
}
