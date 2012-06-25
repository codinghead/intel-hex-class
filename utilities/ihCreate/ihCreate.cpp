/*******************************************************************************
*
* INTEL HEX UTILITY - IHCREATE
*
*******************************************************************************/

/*******************************************************************************
* ihCreate.cpp
*
* This utility is designed to create Intel HEX file format files as per the 
* user's wish.
*
* The utility generates a single file:
* 	- with the value 0xFF between a start and end address
*	- with a chosen by value between a start and end address
*	- with random byte values between a start and end address
*	- with the content of a CSV file (with comma or semicolon delimiting) 
*     starting with user's starting address and ending once the data has been
*     used, or until the end address is reached
*	- with the user's EIP register content OR IP/ES register content
*
* The utility generates the file 'output.hex' when no other filename has been 
* defined.
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
* No notes to date (19th May 2012)
*******************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <vector>

#include "intelhexclass.h"

using namespace std;

char *program_name;

// Notes which arguments were registered
struct IHCREATE_ARGS
{
    bool foundStartAddress;
    bool foundEndAddress;
    bool foundFillValue;
    bool foundRandomData;
    bool foundOutputFileName;
    bool foundInputFileName;
    bool foundEipAddress;
    bool foundEpAddress;
    bool foundCsAddress;
} ihCreateArgs;

// Usage for this program
void usage()
{
    cerr << "Usage is " << program_name <<
                 " -s [start address] -e [end address]" << endl;
    exit (EXIT_FAILURE);
}

/* Converts a hex value in a string into an unsigned long                     */
unsigned long stringToHex(string hexString)
{
    unsigned long hexValue = 0;
    
    /* First check that the string is not empty                               */
    if(hexString.size() != 0)
    {
        string::iterator it;
        
        for (it = hexString.begin(); it != hexString.end(); ++it)
        {
            cout << *it << endl;
            
            /* Shift value left 4 bits                                        */
            hexValue <<= 4;
            /* Add next hex value                                             */
            if (*it >= '0' && *it <= '9')
            {
                hexValue += static_cast<unsigned long>(*it - '0');
            }
            else if (*it >= 'a' && *it <= 'f')
            {
                hexValue += static_cast<unsigned long>(*it - 'a' + 10);
            }
            else if (*it >= 'A' && *it <= 'F')
            {
                hexValue += static_cast<unsigned long>(*it - 'A' + 10);
            }
        }
    }
    
    return hexValue;
}

int main(int argc, char *argv[])
{
    // Create space to hold the arguments
    vector<string> arguments;
    
    // Create an output stream
	ofstream intelHexInput;
	
	// Create a variable for the intel hex data
	intelhex ihCreate;
	
	// Store the start and end addresses required
	unsigned long startAddress = 0UL;
	unsigned long endAddress = 0UL;
		
	// The program name is the first argument - save for later use
	program_name = argv[0];

    // Note that we haven't found any arguments yet
    ihCreateArgs.foundStartAddress = false;
    ihCreateArgs.foundEndAddress = false;
    
	// Decode commands; we need at least two commands to be able to do something
	if(argc < 2) {
    	usage();
    }
    
    // Copy all arguments into vector
    for (int parse = 1; parse < argc; parse++)
    {
        arguments.push_back(argv[parse]);
    }
    
    int argumentsSize = arguments.size();
    
    // Evaluate the arguments
    for (int parse = 0; parse < argumentsSize; parse++)
    {   
        size_t found;
        string searchString;
        string sourceString;
        string contentString;
        
        sourceString = arguments.front();
        
        /* Look for start address                                             */
        searchString = "-s";
        found = sourceString.find(searchString);
        /* If we found string and it is in position zero (i.e. -s0x00000)     */
        if (found!=string::npos && found == 0)
        {
            cout << "Found start address @ " << found << endl;
            
            searchString = "0x";
            found = sourceString.find(searchString);
            if (found!=string::npos && found == 2)
            {
                /* Jump over the 0x part                                      */
                found += 2;
                contentString.erase();
                contentString.assign(sourceString, found, sourceString.size());
                cout << "Address = 0x" << contentString << endl;
                startAddress = stringToHex(contentString);
                cout << "Address = 0x" << uppercase << hex << startAddress << endl;
                
                /* Note that we found the start address                       */
                ihCreateArgs.foundStartAddress = true;
            }
        }
        
        /* Look for end address                                               */
        searchString = "-e";
        found = sourceString.find(searchString);
        /* If we found string and it is in position zero (i.e. -e0x00000)     */
        if (found!=string::npos && found == 0)
        {
            cout << "Found end address @ " << found << endl;

            searchString = "0x";
            found = sourceString.find(searchString);
            if (found!=string::npos && found == 2)
            {
                /* Jump over the 0x part                                      */
                found += 2;
                contentString.erase();
                contentString.assign(sourceString, found, sourceString.size());
                cout << "Address = 0x" << contentString << endl;
                endAddress = stringToHex(contentString);
                cout << "Address = 0x" << uppercase << hex << endAddress << endl;

                /* Note that we found the end address                         */
                ihCreateArgs.foundEndAddress = true;
            }
        }
        
        arguments.erase(arguments.begin());
    }
    
#if 0
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

   /* Check for errors or warnings                                           */
    if (ihStat.getNoWarnings() != 0)
    {
        std::cerr << "File " << argv[1] << " contained warnings." << std::endl;
        decodingIssues = true;
    }
    if (ihStat.getNoErrors() != 0)
    {
        std::cerr << "File " << argv[1] << " contained errors." << std::endl;
        decodingIssues = true;
    }
        
    if (decodingIssues == true)
    {
        std::cerr << "Continuing with stat despite issues listed." << std::endl;
    }
    
    /* Determine start and end addresses                                     */
    ihStat.begin();
	startAddress = ihStat.currentAddress();
	ihStat.end();
	endAddress = ihStat.currentAddress();
	
	cout << "Start address: 0x" << hex << setw(8) << setfill('0') \
		 << uppercase << startAddress << endl;
	cout << "End address  : 0x" << hex << setw(8) << setfill('0') \
		 << uppercase << endAddress << endl;
	cout << "Address range: " << dec << ((endAddress - startAddress) +1UL) \
		 << " bytes" << endl;
	
	/* Determine number of bytes in file                                     */
	cout << "Data bytes   : " << dec << ihStat.size() << endl;
	
	/* If EIP register was found, output value                               */
	unsigned long eipRegister;

	if (ihStat.getStartLinearAddress(&eipRegister))
	{
		cout << "EIP      : 0x" << hex << setw(8) << setfill('0') \
			 << uppercase << eipRegister << endl;
	}

	/* If IP & CS registers were found, output values                        */
	unsigned short ipRegister;
	unsigned short csRegister;

	if (ihStat.getStartSegmentAddress(&ipRegister, &csRegister))
	{
		cout << "IP       : 0x" << hex << setw(4) << setfill('0') \
			 << uppercase << ipRegister << endl;
		cout << "CS       : 0x" << hex << setw(4) << setfill('0') \
			 << uppercase << csRegister << endl;
	}

	/* Find empty regions                                                    */
	bool foundEmptyRegion = false;
	unsigned long emptyAddressCount = 0;
	unsigned long emptyRegionStart = 0;
	unsigned long emptyRegionEnd = 0;
	unsigned long previousAddress = 0;
	unsigned long currentAddress = 0;
	unsigned char data = 0x00;
	bool inEmptyRegion = false;

	cout << "Finding empty regions..." << endl;

	//for (unsigned long address = startAddress; address <= endAddress; 
	//	                                                            address ++)
	ihStat.begin();
	previousAddress = ihStat.currentAddress();

	do
	{
		++ihStat;

		currentAddress = ihStat.currentAddress();

		if (currentAddress != (previousAddress + 1UL))
		{
			foundEmptyRegion = true;
			emptyRegionStart = previousAddress + 1UL;
			emptyRegionEnd = currentAddress - 1UL;
			emptyAddressCount = (emptyRegionEnd - emptyRegionStart) + 1UL;
			cout << "Between 0x" << hex << uppercase << setw(8) \
				 << emptyRegionStart \
				 << " and 0x" << hex << uppercase << setw(8) \
				 << emptyRegionEnd \
				 << " are " << dec << emptyAddressCount \
				 << " free bytes." << endl;
		}

		previousAddress = currentAddress;
	} while (!ihStat.endOfData());

	if (foundEmptyRegion == false)
	{
		cout << "    ...no empty regions in file." << endl;
	}

#endif	

    return(0);	
}
