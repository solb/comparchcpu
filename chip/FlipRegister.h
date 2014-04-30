// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Defines a shift register that can flip any bit

#ifndef _FLIPREGISTER_H_
#define _FLIPREGISTER_H_
#include <ShiftRegister.h>

class FlipRegister: public virtual ShiftRegister{
    public:
        FlipRegister (const char *id, int numBits, long initVal = 0);
        ~FlipRegister();

        void flipBit( int bit );
        void rightShift();	// shorthand
	    void rightArithShift();	// shorthand
	    void leftShift();	// shorthand

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
