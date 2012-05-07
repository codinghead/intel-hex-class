/*******************************************************************************
*
* INTEL HEX UTILITY - IHITEGRITY
*
*******************************************************************************/

/*******************************************************************************
* ihIntegrity.cpp
*
* This utility is designed to check the integrity of an Intel HEX file. It
* checks the following:
*   - That no address is assigned two different bytes of data
*   - That the checksum for all lines is correct
*   - That the End Of File record appears only once
* Upon completion, if there are no issues found, the program returns nothing.
* If issues are found the program returns a failure. In verbose mode, the
* result of the test will be output to cout. If desired, the result can also be
* output to a text file.
*
* For more information and the latest release, please visit this projects home
* page at http://codinghead.github.com/Intel-HEX-Class
* To participate in the project or for other enquiries, please contact Stuart
* Cording at codinghead@gmail.com
*
* Licensing Information
* Copyright (c) 2012 Stuart Cording
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Author: Stuart Cording aka CODINGHEAD
*
********************************************************************************
* Notes:
* No notes to date (24th Feb 2012)
*******************************************************************************/

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
	string buildDate = __DATE__;
	string buildTime = __TIME__;
	
    cerr << "Usage is " << program_name <<
                 " [file]" << endl;
    
	cerr << "Build date: " << buildDate << " Build time: " << buildTime << endl;
	exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // Create an input stream
	std::ifstream intelHexInput;
	
	// Create a variable for the intel hex data
	intelhex ihIntegrity;
	
	// The program name is the first argument - save for later use
	program_name = argv[0];

	// Make sure there are only <command> and 1 x <file> arguments	
	if(argc != 2) {
    	usage();
    }
    	
	intelHexInput.open(argv[1], ifstream::in);
	
	if(!intelHexInput.good())
	{
    	std::cerr << "Error: couldn't open " << argv[1] << std::endl;
    	usage();
	}
 
    intelHexInput >> ihIntegrity;
    if ((ihIntegrity.getNoWarnings() > 0) || (ihIntegrity.getNoErrors() > 0))
    {
        cout << "Issues found with " << argv[1] << endl;
        
        if (ihIntegrity.getNoErrors() > 0)
            cout << "Errors:" << endl;
        while (ihIntegrity.getNoErrors() > 0)
        {
            string message;
            
            ihIntegrity.popNextError(message);
            
            cout << message << endl;
        }
        
        if (ihIntegrity.getNoWarnings() > 0)
            cout << "Warnings:" << endl;
        while (ihIntegrity.getNoWarnings() > 0)
        {
            string message;
            
            ihIntegrity.popNextWarning(message);
            
            cout << message << endl;
        }
    }
    else
    {
        cout << "No errors found in " << argv[1] << endl;
    }
       
    return(0);	
}
