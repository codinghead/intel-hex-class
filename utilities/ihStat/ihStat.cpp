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
*	- Total number of bytes in file
*	- Regions and/or locations that contain no data entries
*	- Values stored in IP/ES or EIP registers (if present)
*	- Regions for which there is no data available
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

#include "../../intelhex_class/intelhexclass.h"

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
    if(argc != 2)
    {
        usage();
    }
    	
    intelHexInput.open(argv[1], ifstream::in);
	
    if(!intelHexInput.good())
    {
        cerr << "Error: couldn't open " << argv[1] << endl;
        usage();
    }
	
    /* Output file names                                                      */
    cout << "Statistic for file: " << argv[1] << endl;
    
    /* Decode file                                                            */
    intelHexInput >> ihStat;

    /* Check for errors and abort                                             */
    if (ihStat.getNoErrors() != 0)
    {
        cerr << "File " << argv[1] << " contained errors:" << endl;

        while (ihStat.getNoErrors() > 0) {
            string message;
            ihStat.popNextError(message);
            cerr << message << endl;
        }
        return(EXIT_FAILURE);
    }
        
    /* Check for warnings and continue                                        */
    if (ihStat.getNoWarnings() != 0)
    {
        cout << "File " << argv[1] << " contained warnings:" << endl;
        decodingIssues = true;

        while (ihStat.getNoWarnings() > 0) {
            string message;
            ihStat.popNextWarning(message);
            cout << message << std::endl;
        }
    }

    if (decodingIssues == true)
    {
        cout << "Continuing with stat despite warnings listed." << endl;
    }

    /* Start the statistical analysis                                        */
    /* Determine start and end addresses                                     */
    ihStat.startAddress(&startAddress);
    ihStat.endAddress(&endAddress);

    cout << "Start address: 0x" << hex << setw(8) << setfill('0') \
        << uppercase << startAddress << endl;
    cout << "End address  : 0x" << hex << setw(8) << setfill('0') \
        << uppercase << endAddress << endl;
    cout << "Address range: " << dec << ((endAddress - startAddress) + 1UL) \
        << " bytes (0x" << hex << setw(8) << uppercase \
        << setfill('0') << ((endAddress - startAddress) + 1UL) << ")" << endl;
	
    /* Determine number of bytes in file                                     */
    cout << "Data bytes   : " << dec << ihStat.size() \
        << " bytes (0x" << hex << setw(8) << uppercase \
        << setfill('0') << ihStat.size() << ")" << endl;

    /* If EIP register was found, output value                               */
    unsigned long eipRegister;

    if (ihStat.getStartLinearAddress(&eipRegister))
    {
        cout << "EIP      : 0x" << hex << setw(8) << setfill('0') \
            << uppercase << eipRegister << endl;
    }
    else
    {
        cout << "EIP      : Not found" << endl;
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
    else
    {
        cout << "IP       : Not found" << endl;
        cout << "CS       : Not found" << endl;
    }

    /* Find empty regions                                                    */
    bool foundEmptyRegion = false;
    unsigned long emptyAddressCount = 0;
    unsigned long emptyRegionStart = 0;
    unsigned long emptyRegionEnd = 0;
    unsigned long previousAddress = 0;
    unsigned long currentAddress = 0;
//    unsigned char data = 0x00;
//    bool inEmptyRegion = false;

    cout << "Finding empty regions..." << endl;

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

    return(EXIT_SUCCESS);	
}
