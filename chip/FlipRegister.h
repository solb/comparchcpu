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

// Defines a shift register that can flip any bit

#ifndef _FLIPREGISTER_H_
#define _FLIPREGISTER_H_
#include <ShiftRegister.h>
#include <CPUObject.h>
#include <StorageObject.h>
class FlipRegister: public virtual ShiftRegister{
    public:
        FlipRegister (const char *id, int numBits, long initVal = 0);
        ~FlipRegister();

        void flipBit( int bit );
        void rightShift();	// shorthand
	    void rightArithShift();	// shorthand
	    void leftShift();	// shorthand
        void clear();

        void perform( ShiftRegister::Operation );
    protected:
        void phase1();
    private:
        int bitsToFlip;
        int flipBits;

};
inline void FlipRegister::rightShift() { perform( right ); }

inline void FlipRegister::rightArithShift() { perform( rightArith ); }

inline void FlipRegister::leftShift() { perform( left ); }
#endif
