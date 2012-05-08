/*******************************************************************************
*
* INTEL HEX UTILITY - IHREFACTOR
*
*******************************************************************************/

/*******************************************************************************
* ihRefactor.cpp
*
* This utility is designed to refactor the content of an Intel HEX file. Often,
* the content of an Intel HEX file is not created in increasing address order
* but in several chunks of address blocks where higher addressed blocks come
* before lower addressed blocks. Occaisionally files can also have a number of 
* Linear/Segment Address Records although the entire file content lies in the 
* same 64kByte segment and requires only a single such record.
* This utility reads the desired hex file into memory and then writes it anew 
* into the desired destination file. The resulting file will be built from the 
* lowest address found in the source file to the highest with the minimum
* number of Linear/Segment Address Records required. 
* The resulting file is, content-wise, the same as the source file. However, 
* examining the content in a text editor should be made easier since the code is
* in the order that it would appear in memory.
* The utility also checks the source file for issues. If any warnings occur, the
* destination will still be created and the warnings will be output to cout.
* If there are any serious issues with the source file, no destination file will
* be created. The errors found will be listed to cout.
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
    cerr << "Usage is " << program_name <<
                 " [input_file_a] [input_file_b]" << endl;
    exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // Create an input stream
	ifstream intelHexInputA;
	ifstream intelHexInputB;
	
	// Create an output stream
	ofstream intelHexOutput;
	
	// Create a variable for the intel hex data
	intelhex ihDiffA;
	intelhex ihDiffB;
	
	// The program name is the first argument - save for later use
	program_name = argv[0];

	// Make sure there are only <command> and 2 x <file> arguments	
	if(argc != 3) {
    	usage();
    }
    	
	intelHexInputA.open(argv[1], ifstream::in);
	
	if(!intelHexInputA.good())
	{
    	std::cerr << "Error: couldn't open " << argv[1] << std::endl;
    	usage();
	}
	
	intelHexInputB.open(argv[2], ifstream::in);
	
	if(!intelHexInputB.good())
	{
    	std::cerr << "Error: couldn't open " << argv[2] << std::endl;
    	usage();
	}
 
    intelHexInputA >> ihDiffA;
    intelHexInputB >> ihDiffB;
    
    if (ihDiffA.getNoErrors() > 0 || ihDiffB.getNoErrors() > 0)
    {
        if (ihDiffA.getNoErrors() > 0)
        {
            cout << "Errors found in " << argv[1] << endl;
        
            while (ihDiffA.getNoErrors() > 0)
            {
                string message;
                
                ihDiffA.popNextError(message);
                
                cout << message << endl;
            }
        }
        
        if (ihDiffB.getNoErrors() > 0)
        {
            cout << "Errors found in " << argv[2] << endl;
        
            while (ihDiffB.getNoErrors() > 0)
            {
                string message;
                
                ihDiffB.popNextError(message);
                
                cout << message << endl;
            }
        }
        
        cout << "No diff performed." << endl;
        
        exit (EXIT_FAILURE);
    }
    
    
       
    return(0);	
}
