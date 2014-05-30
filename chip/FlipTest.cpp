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
    test.flipBit(3);
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    test.perform(ShiftRegister::left); 
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;
    test.clear();
    Clock::tick();
    cout << bitset<5>(test.value())<<endl;   
}
