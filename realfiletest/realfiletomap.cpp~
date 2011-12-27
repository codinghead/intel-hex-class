/* Opens an Intel Hex file and reads it in line by line */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream> // for string streaming
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

char *program_name;

// Usage for this program
void usage()
{
    std::cerr << "Usage is " << program_name <<
                 " [file]" << std::endl;
    exit (8);
}

unsigned char charToHex(char value)
{
    //std::cout << "In charToHex()" << std::endl;
    
    if (value >= '0' && value <= '9')
    {
        return static_cast<unsigned char>(value - '0');
    }
    else if (value >= 'A' && value <= 'F')
    {
        return static_cast<unsigned char>(value - 'A' + 10);
    }
    else if (value >= 'a' && value <= 'f')
    {
        return static_cast<unsigned char>(value - 'a' + 10);
    }
    else
    {
        std::cout << "Error: In charToHex()" << std::endl;
        return 0xFF;
    }
}

int main(int argc, char *argv[])
{	
    map<int, unsigned char> hexFileMap;
    map<int, unsigned char>::iterator hexFileMapIterator;
    pair<map<int, unsigned char>::iterator,bool> hexFileMapReturn;
    
	// Create an input stream
	std::ifstream intelHexInput;
	// Create a string to store lines of Intel Hex info
	std::string intelHexLine;
	
	// The program name is the first argument - save for later use
	program_name = argv[0];

	// Make sure there is only <command> and <file> arguments	
	if(argc != 2) {
    	usage();
    }
    	
	intelHexInput.open(argv[1], std::ifstream::in);
	
	if(intelHexInput.bad())
	{
    	std::cerr << "Error: couldn't open " << argv[1] << std::endl;
    	usage();
	}

    int totalLines = 0;
    std::vector<unsigned char> lineData;
    
    do {
        std::string::iterator it;
        
        intelHexInput >> intelHexLine;
        ++totalLines;
        
        // Set string iterator to start of string
        it = intelHexLine.begin();
        
        // Test first position for the start code ':'
        if (*it != ':') {
            std::cout << "Error: Start code \':\' missing on line " << std::dec << totalLines << std::endl;
            break;
        }
        
        // Remove the start code from the string as we don't need it anymore
        intelHexLine.erase(it);
        
        unsigned char intelHexChecksum = 0;
        
        std::cout << std::dec << std::setw(6) << totalLines << ":" << intelHexLine << std::endl << "      :" ;
        
        for (it = intelHexLine.begin(); it != intelHexLine.end(); /* nothing */ ) {
            unsigned char upperNibble = 0xFF;
            unsigned char lowerNibble = 0xFF;
            std::string byteReadTemp;
            unsigned char byteRead = 0;
            
            upperNibble = charToHex(static_cast<char>(*it));
            byteReadTemp = *it;
            
            ++it;
            
            lowerNibble = charToHex(*it);
            byteReadTemp += *it;
            
            ++it;
            
            if (upperNibble > 0x0F) {
                std::cout << "Error: Upper nibble in line " << std::dec <<  totalLines << " of byte " << std::hex << byteReadTemp << " not hexedecimal value." << std::endl;
            } else if (lowerNibble > 0x0F) {
                std::cout << "Error: Lower nibble in line " << std::dec << totalLines << " of byte " << std::hex << byteReadTemp << " not hexedecimal value." << std::endl;
            } else {
                byteRead = (upperNibble << 4) + lowerNibble;
            }
            intelHexChecksum += byteRead;
       
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned int>(byteRead);
            byteReadTemp.erase();
        }

        std::cout << std::endl;
        
        if(intelHexChecksum != 0) {
            std::cout << "Error: Checksum error on line " << std::dec << totalLines << " of " << std::hex << static_cast<unsigned int>(intelHexChecksum) << std::endl;
        }
        
        intelHexLine.erase();
                
    } while (!intelHexInput.eof());
	
	std::cout << "Total lines = " << std::dec << totalLines << std::endl;
	
	return(0);
}

