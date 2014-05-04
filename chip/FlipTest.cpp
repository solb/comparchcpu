
#include <ArchLibError.h>
#include <iostream>
#include <bitset>
#include "FlipRegister.h"
#include <Clock.h>
#include <ShiftRegister.h>

using namespace std;

int main(){
    FlipRegister test( "Test", 5, 31 );
    cout << bitset<5>(test.value())<<endl;
    test.flipBit(2);
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    test.flipBit(2);
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    test.perform(ShiftRegister::left); 
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    test.clear();
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;   
}
