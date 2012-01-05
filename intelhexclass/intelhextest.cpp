#include <iostream>
#include <fstream>
#include <cstdlib>

#include "intelhexclass.hpp"

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
    
    cout << "Final address is " << classTest.currentAddress() << endl;
    
    cout << "File contained " << classTest.noOfWarnings << " warnings and "
         << classTest.noOfErrors << " errors." << endl;
    
    if (classTest.noOfErrors > 0)
    {
        classTest.ihErrors.begin();
        
        while(classTest.noOfErrors > 0)
        {
            cout << classTest.ihErrors.front() << endl;
            --classTest.noOfErrors;
            classTest.ihErrors.pop_front();
        }
    }
    
    if (classTest.noOfWarnings > 0)
    {
        classTest.ihWarnings.begin();
        
        while(classTest.noOfWarnings > 0)
        {
            cout << classTest.ihWarnings.front() << endl;
            --classTest.noOfWarnings;
            classTest.ihWarnings.pop_front();
        }
    }
    
    return(0);
}

