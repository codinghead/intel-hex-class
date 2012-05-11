/*******************************************************************************
*
* INTEL HEX UTILITY - IHSTAT
*
*******************************************************************************/

/*******************************************************************************
* ihStat.cpp
*
* This utility is designed to calculate basic statistics for a given Intel HEX
* format file.
*
* The utility generates:
* 	- First address found
*	- Last address found
*	- Total size of this region
*	- Regions and/or locations that contain no data entries
*	- Values stored in IP/ES or EIP registers
*	- Total number of bytes in file
*
* The utility also checks the source file for issues. Regardless of the issues
* potentially found, the utility will do its best to generate a list of
* statistics. These issues will also be output as a list of errors and 
* warnings.
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
* No notes to date (9th May 2012)
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
                 " [input_file]" << endl;
    exit (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // Create an input stream
	ifstream intelHexInput;
	
	// Create a variable for the intel hex data
	intelhex ihStat;
	
	// Note if there were issues with decoding
	bool decodingIssues = false;
	
	// Storage for the first and last addresses in the file
	unsigned long startAddress = 0UL;
	unsigned long endAddress = 0UL;
		
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
	
	/* Output file names                                                      */
    cout << "Statistic for file: " << argv[1] << endl;
    
    /* Decode file                                                            */
    intelHexInput >> ihStat;

#error Got here
    
    /* Check for errors or warnings                                           */
    if (intelHexInput.getNoWarnings() != 0)
    {
        std::cerr << "File " << argv[1] << " contained warnings." << std::endl;
        decodingIssues = true;
    }
    if (intelHexInput.getNoErrors() != 0)
    {
        std::cerr << "File " << argv[1] << " contained errors." << std::endl;
        decodingIssues = true;
    }
        
    if (decodingIssues == true)
    {
        std::cerr << "Continuing with stat despite issues listed." << std::endl;
    }
    
    /* Determine start and end addresses                                     */
    intelHexInput.begin();
	startAddress = intelHexInput.currentAddress();
	intelHexInput.end();
	endAddress = intelHexInput.currentAddress();
	
	cout << "Start address: 0x" << hex << setw(8) << startAddress << endl;
	cout << "End address  : 0x" << hex << setw(8) << endAddress << endl;
	cout << "Address range: " << dec << ((endAddress - startAddress) +1) << \
	        " bytes" << endl;
	
	/* Determine number of bytes in file                                     */
	cout << "Data bytes   : " << dec << intelHexInput.size() << endl;
	
    /* Two local var's to store data at the locations being analysed         */
    unsigned char diffAData = 0;
    unsigned char diffBData = 0;
    
    bool aFinished = false;
    bool bFinished = false;
    
    bool complete = false;
    
    do
    {
        /* Get address of data we are pointing to                             */
        diffAAddress = ihDiffA.currentAddress();
        diffBAddress = ihDiffB.currentAddress();
        
        /* Get the data at two current addresses                              */
        ihDiffA.getData(&diffAData);
        ihDiffB.getData(&diffBData);
        
        /* If addresses are the same, compare data values                     */
        if (diffAAddress == diffBAddress)
        {
            if (diffAData != diffBData)
            {
                cout << uppercase << "0x" \
                     << hex << setw(8) << setfill('0') << diffAAddress \
                     << "         0x" << hex << setw(2) \
                     << static_cast<unsigned short>(diffAData) \
                     << "         0x" << hex << setw(2) \
                     << static_cast<unsigned short>(diffBData) << endl;
            }
            /* Increment both addresses                                       */
            ++ihDiffA;
            ++ihDiffB;
        }
        
        /* If addresses are different, find out which one is lower and output */
        /* that this address has data where the other does not have data      */
        else if (diffAAddress < diffBAddress)
        {
            /* Output A address as reference address since this exists and    */
            /* the B address doesn't                                          */
            cout << uppercase << "0x" \
                 << hex << setw(8) << setfill('0') << diffAAddress \
                 << "         0x" << hex << setw(2) \
                 << static_cast<unsigned short>(diffAData) \
                 << "         ----" << endl;
            ++ihDiffA;
        }
        else
        {
            /* Here we output the B address as reference address since data   */
            /* appears at this address in file B, but not in file A           */
            cout << uppercase << "0x" \
                 << hex << setw(8) << setfill('0') << diffBAddress \
                 << "         ----" \
                 << "         0x" << hex << setw(2) \
                 << static_cast<unsigned short>(diffBData) << endl;
            ++ihDiffB;
        }
        
        /* Check if we got to the end of the two files                        */
        if (ihDiffA.endOfData() == true)
        {
            break;
        }
        else if (ihDiffB.endOfData() == true)
        {
            break;
        }
        
    } while (complete != true);
       
    return(0);	
}
