// Sol Boucher and Evan Klei
// CSCI-453-01 Computer Architecture
// FabComp: implementation

// Main program for the simulation

#include "control_path.h"
#include "data_path.h"
#include "microcontroller.h"
#include <ArchLibError.h>
#include <CPUObject.h>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::hex;

// Validate number of command-line arguments and run the simulator
int main(int argc, const char *argv[]) {
	if(argc != 4) {
		cout << "USAGE: " << argv[0] <<
				" <microcode> <micro--jump table> <program>" << endl;
		cout << "<microcode> usually ends in the extension .fu" << endl;
		cout << "<micro--jump table> usually ends in the extension .fl" << endl;
		cout << "<program> usually ends in the extension .fo" << endl;
		return 1;
	}

	cout << hex;
	cerr << hex;

    #ifdef DEBUG_ARCH
    CPUObject::debug |= CPUObject::trace;
    #endif

	try {
		wire_data_path();
		wire_control_path();

		load_microprogram(argv[1], argv[2]);
		load_userprogram(argv[3]);

		interpret_microprogram();

		melt_data_path();
		melt_control_path();
	}
	catch(const ArchLibError &ex) {
		cerr << "ArchLib runtime error!" << endl;
		cerr << "Cause: " << ex.what() << endl;
		return 2;
	}
}
