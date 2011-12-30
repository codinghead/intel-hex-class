#include <iostream>
#include <string>

#include "intelhexclass.hpp"

using namespace std;

/* Constructor                                                                */
intelhex::intelhex()
{
    /* Initialise the address pointer to zero                                 */
    currentAddress = 0;
    /* Clear content of register variables used with the 'Start Segment' and  */
    /* 'Start Linear' address records                                         */
    ipRegister = 0;
    csRegister = 0;
    eipRegister = 0;
}

/* Destructor                                                                 */
intelhex::~intelhex()
{
    
}

/* Converts ASCII chars to their HEX values                                   */
unsigned char charToHex(char value)
{
    // cout << "In charToHex()" << value << endl;
    
    if (value >= '0' && value <= '9')
    {
        //std::cout << "A:Found " << value << " returning " << static_cast<unsigned int>(value - '0') << std::endl;
        return static_cast<unsigned char>(value - '0');
    }
    else if (value >= 'A' && value <= 'F')
    {
        //std::cout << "B:Found " << value << " returning " << static_cast<unsigned int>(value - 'A' + 10) << std::endl;
        return static_cast<unsigned char>(value - 'A' + 10);
    }
    else if (value >= 'a' && value <= 'f')
    {
        //std::cout << "C:Found " << value << " returning " << static_cast<unsigned int>(value - 'a' + 10) << std::endl;
        return static_cast<unsigned char>(value - 'a' + 10);
    }
    else
    {
        /* Add some error code here                                           */
        //cerr << "Error: In charToHex()" << endl;
        return 0xFF;
    }
}

/* Input Stream for Intel HEX File Decoding                                   */
istream& operator>>(istream& dataIn, const intelhex& intelhexData)
{
    // Create a string to store lines of Intel Hex info
	std::string ihLine;
	// Create an iterator for this variable
    std::string::iterator ihLineIterator;
    // Create a line counter
    unsigned long lineCounter = 0;
    /* Variables to hold upper and lower nibbles of data being read during    */
    /* decoding                                                               */
    unsigned char upperNibble;
    unsigned char lowerNibble;
    // Variable to hold a single byte (two chars) of data
    unsigned char byteRead;
    // Variable to calculate the checksum for each line
    unsigned char intelHexChecksum;
    // Variable to hold the record length
    unsigned char recordLength;
    // Variable to hold the load offset
    unsigned long loadOffset;
    // Variables to hold the record type
    enum intelhexRecordType recordType;
    unsigned char tempRecordType;
    
    do
    {
        /* Clear the string before this next round                            */
        ihLine.erase();
        
        /* Clear the checksum before processing this line                     */
        intelHexChecksum = 0;
        
        /* Get a line of data                                                 */
        dataIn >> ihLine;
        
        /* If the line contained some data, process it                        */
        if (ihLine.length() > 0)
        {
            /* Increment line counter                                         */
            lineCounter++;
        
            /* Set string iterator to start of string                         */
            ihLineIterator = ihLine.begin();
        
            /* Check that we have a ':' record mark at the beginning          */
            if (*ihLineIterator != ':')
            {
                /* Add some error code here                                   */
                cerr << "Line without record mark ':' found @" << lineCounter
                                                                        << endl;
            }
        
            /* Remove the record mark from the string as we don't need it     */
            /* anymore                                                        */
            ihLine.erase(ihLineIterator);
        
            /* Run through the whole line to check the checksum               */
            for (ihLineIterator = ihLine.begin(); 
                 ihLineIterator != ihLine.end(); 
                 /* Nothing - really! */ )
            {
                /* Convert the line in pair of chars (making a single byte)   */
                /* into single bytes, and then add to the checksum variable.  */
                /* By adding all the bytes in a line together *including* the */
                /* checksum byte, we should get a result of '0' at the end.   */
                /* If not, there is a checksum error                          */
                upperNibble = charToHex(static_cast<char>(*ihLineIterator));
            
                ++ihLineIterator;
            
                lowerNibble = charToHex(static_cast<char>(*ihLineIterator));
            
                ++ihLineIterator;
            
                byteRead = (upperNibble << 4) + lowerNibble;
            
                intelHexChecksum += byteRead;
            }
        
            /* Make sure the checksum was ok                                  */
            if (intelHexChecksum == 0)
            {
                /* Reset iterator back to beginning of the line so we can now */
                /* decode it                                                  */
                ihLineIterator = ihLine.begin();
                
                /* Clear all the variables associated with decoding a line of */
                /* Intel HEX code.                                            */
                recordLength = 0;
                tempRecordType = 0;
                loadOffset = 0;
                
                /* Get the record length                                      */
                upperNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                lowerNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                recordLength = (upperNibble << 4) + lowerNibble;
                
                /* Get the load offset (2 bytes)                              */
                upperNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                lowerNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                loadOffset = 
                   static_cast<unsigned long>((upperNibble << 4) + lowerNibble);
                loadOffset <<= 8;
                upperNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                lowerNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                loadOffset += 
                   static_cast<unsigned long>((upperNibble << 4) + lowerNibble);
                
                /* Get the record type                                        */
                upperNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                lowerNibble = charToHex(static_cast<char>(*ihLineIterator));
                ++ihLineIterator;
                tempRecordType = (upperNibble << 4) + lowerNibble;
                
                /* Convert record type to proper type                         */
                recordType = 
                           static_cast<enum intelhexRecordType>(tempRecordType);
                           
                /* Decode the INFO or DATA portion of the record              */
                switch (recordType)
                {
                    case DATA_RECORD:
                        //decodeDataRecord();
                        break;
                    
                    case END_OF_FILE_RECORD:
                        /* Note the line number at this point. Later, check   */
                        /* that there were no more lines after this one.      */
                        /* Generate error if there were                       */
                        break;
                        
                    case EXTENDED_SEGMENT_ADDRESS:
                        /* Make sure we have 2 bytes of data                  */
                        if (recordLength == 2)
                        {
                        }
                        else
                        {
                            /* Note the error                                 */
                        }
                        break;
                        
                    case START_SEGMENT_ADDRESS:
                        /* Make sure we have 4 bytes of data                  */
                        if (recordLength == 4)
                        {
                        }
                        else
                        {
                            /* Note the error                                 */
                        }
                        break;
                        
                    case EXTENDED_LINEAR_ADDRESS:
                        /* Make sure we have 2 bytes of data                  */
                        if (recordLength == 2)
                        {
                        }
                        else
                        {
                            /* Note the error                                 */
                        }
                        break;
                        
                    case START_LINEAR_ADDRESS:
                        /* Make sure we have 4 bytes of data                  */
                        if (recordLength == 4)
                        {
                        }
                        else
                        {
                            /* Note the error                                 */
                        }
                        break;
                    
                    default:
                        /* Handle the error here                              */
                        break;
                }
            }
            else
            {
                /* Add some error code here                                   */
                cerr << "Checksum error on line " << lineCounter << endl;
            }
        }
    } while (ihLine.length() > 0);
    
    cout << ihLine << endl << lineCounter << endl;
    
    return(dataIn);
}
