#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include "intelhexclass.h"

using namespace std;

char *program_name;

// Usage for this program
void usage()
{
    cerr << "Usage is " << program_name <<
                 " [file]" << endl;
    exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // Create an input stream
	std::ifstream intelHexInput;
	// Create a string to store lines of Intel Hex info
	std::string intelHexLine;
	// Create a variable of the class we are testing
	intelhex classTest;
	
	// Make class verbose
	classTest.verboseOn();
	
	// The program name is the first argument - save for later use
	program_name = argv[0];

	// Make sure there is only <command> and <file> arguments	
	if(argc != 2) {
    	usage();
    }
    	
	intelHexInput.open(argv[1], ifstream::in);
	
	if(!intelHexInput.good())
	{
    	std::cerr << "Error: couldn't open " << argv[1] << std::endl;
    	usage();
	}
	
    cout << "Running IntelHEX Class Test" << endl;
    
    intelHexInput >> classTest;
    
    cout << "Final address is 0x" << setw(2) << setfill('0') << uppercase << hex << classTest.currentAddress() << endl;

    cout << "File contained " << classTest.getNoWarnings() << " warnings and "
         << classTest.getNoErrors() << " errors." << endl;
    
    while (classTest.getNoErrors() > 0)
    {
        string message;
        
        classTest.popNextError(message);
        
        cout << message << endl;
    }
    
    while (classTest.getNoWarnings() > 0)
    {
        string message;
        
        classTest.popNextWarning(message);
        
        cout << message << endl;
    }
    
    return(0);
}

