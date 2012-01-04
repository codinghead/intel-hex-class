#include <iostream>
#include <string>
#include <vector>

#include "intelhexclass.hpp"

using namespace std;

enum intelhexRecordType {
    DATA_RECORD,
    END_OF_FILE_RECORD,
    EXTENDED_SEGMENT_ADDRESS,
    START_SEGMENT_ADDRESS,
    EXTENDED_LINEAR_ADDRESS,
    START_LINEAR_ADDRESS,
    NO_OF_RECORD_TYPES
};

/* Converts a 2 char string to its HEX value                                  */
unsigned char intelhex::stringToHex(string value)
{
    unsigned char returnValue = 0;
    string::iterator valueIterator;
    
    if(value.length() == 2)
    {
        //cout << "In stringToHex() " << value << " ";
        valueIterator = value.begin();
        
        for (int x=0; x < 2; x++)
        {
            /* Shift result variable 4 bits to the left                       */
            returnValue <<= 4;
            
            if (*valueIterator >= '0' && *valueIterator <= '9')
            {
                //cout << " 0-9 ";
                returnValue += 
                               static_cast<unsigned char>(*valueIterator - '0');
            }
            else if (*valueIterator >= 'A' && *valueIterator <= 'F')
            {                
                //cout << " A-F ";
                returnValue += 
                          static_cast<unsigned char>(*valueIterator - 'A' + 10);
            }
            else if (*valueIterator >= 'a' && *valueIterator <= 'f')
            {
                //cout << " a-f ";
                returnValue += 
                          static_cast<unsigned char>(*valueIterator - 'a' + 10);
            }
            else
            {
                /* Add some error code here                                   */
                string message;

                message = "Can't convert byte 0x" + value + " @ 0x" + 
                          ulToHexString(segmentBaseAddress) + " to hex.";

                addError(message);
                
                returnValue = 0;
            }
            ++valueIterator;
        }
    }
    else
    {
        string message;

        message = value + " @ 0x" + ulToHexString(segmentBaseAddress) + 
                                                       " isn't an 8-bit value.";

        addError(message);
    }
    //cout << returnValue << ";" << endl;
    
    return returnValue;
}

/* Converts an unsigned long char to a string in HEX format                   */
string intelhex::ulToHexString(unsigned long value)
{
    string returnString;
    
    returnString.erase();
    
    /* Converts values from 0x00000000 to 0xFFFFFFFF only                     */
    for (int x = 0; x < 8; x++)
    {
        /* Mask off lsByte                                                    */
        char byte = static_cast<char>(value & 0xF);
        
        returnString.insert(0, 1, (byte + '0'));
        
        /* Shift the original value 8 bits to the right to get next byte      */
        value >>= 8;
    }
    
    return returnString;
}

string intelhex::ulToString(unsigned long value)
{
    string returnString;
    
    returnString.erase();
    
    if (value == 0)
    {
        returnString.insert(0, 1, '0');    
    }
    else
    {
        unsigned long modulo = 10;
        while (value != 0)
        {
            unsigned long ulTemp;
            char cTemp;
            
            ulTemp = value - (value % modulo);
            cTemp = static_cast<char>(ulTemp);
            
            returnString.insert(returnString.end(), 1, (cTemp + '0'));
            
            modulo *= 10;
        }    
    }
    
    return returnString;
}

string intelhex::ucToHexString(unsigned char value)
{
    string returnString;
    
    returnString.erase();
    
    /* Converts values from 0x00 to 0xFF only                                 */
    for (int x = 0; x < 2; x++)
    {
        /* Mask off lsByte                                                    */
        char byte = static_cast<char>(value & 0xF);
        
        returnString.insert(0, 1, (byte + '0'));
        
        /* Shift the original value 8 bits to the right to get next byte      */
        value >>= 8;
    }
    
    return returnString;
}

void intelhex::addWarning(string warningMessage)
{
    string localMessage;
    
    /* Increment the number of warning messages                               */
    ++noOfWarnings;
    
    localMessage += noOfWarnings + " Warning: " + warningMessage;
    
    ihWarnings.insert(ihWarnings.end(), localMessage);
}

void intelhex::addError(string errorMessage)
{
    string localMessage;
    
    /* Increment the number of error messages                                 */
    ++noOfErrors;
    
    localMessage += noOfErrors + " Error: " + errorMessage;
    
    ihErrors.insert(ihErrors.end(), localMessage);
}

void intelhex::decodeDataRecord(unsigned char recordLength,
                                unsigned long loadOffset,
                                string::const_iterator data)
{
    /* Variable to store a byte of the record as a two char string            */
    string sByteRead;
    
    /* Variable to store the byte of the record as an u.char                  */
    unsigned char byteRead;
    
    /* Calculate new SBA                                                      */
    segmentBaseAddress += loadOffset;
    
    for (int x = 0; x < static_cast<int>(recordLength); x ++)
    {
        sByteRead.erase();
        
        sByteRead = *data;
        data++;
        sByteRead += *data;
        data++;

        byteRead = stringToHex(sByteRead);
        
        ihReturn=ihContent.insert(pair<int,unsigned char>(segmentBaseAddress, byteRead)); 
        
        if (ihReturn.second==false)
        {
            /* If this address already contains the byte we are trying to     */
            /* write, this is only a warning                                  */
            if (ihReturn.first->second == byteRead)
            {
                string message;

                message = "Location 0x" + ulToHexString(segmentBaseAddress) + 
                                        " already contains data 0x" + sByteRead;
                                        
                addWarning(message);
            }
            /* Otherwise this is an error                                     */
            else
            {
                string message;

                message = "Couldn't add 0x" + sByteRead + " @ 0x" + 
                          ulToHexString(segmentBaseAddress) + 
                               "; already contains 0x" + 
                                          ucToHexString(ihReturn.first->second);

                addError(message);
            }
#if 0
          cout << "element " << segmentBaseAddress << " already existed";
          cout << " with a value of "; //<< ret.first->second << endl;
          cout << "0x" << static_cast<unsigned short>(ihReturn.first->second);
          cout << " tried writing 0x" << static_cast<unsigned short>(byteRead) << endl;
#endif          
        }
        
        segmentBaseAddress++;
#if 0
        cout << *data;
        data++;
        cout << *data;
        data++;
#endif
    }
#if 0    
    cout << endl;
#endif
}

/* Input Stream for Intel HEX File Decoding (friend function)                 */
istream& operator>>(istream& dataIn, intelhex& ihLocal)
{
    // Create a string to store lines of Intel Hex info
	string ihLine;
	/* Create a string to store a single byte of Intel HEX info               */
	string ihByte;
	// Create an iterator for this variable
    string::iterator ihLineIterator;
    // Create a line counter
    unsigned long lineCounter = 0;
    // Variable to hold a single byte (two chars) of data
    unsigned char byteRead;
    // Variable to calculate the checksum for each line
    unsigned char intelHexChecksum;
    // Variable to hold the record length
    unsigned char recordLength;
    // Variable to hold the load offset
    unsigned long loadOffset;
    // Variables to hold the record type
    intelhexRecordType recordType;
    
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
                /* Add some warning code here                                 */
                string message;

                message = "Line without record mark ':' found @ line " +
                                                ihLocal.ulToString(lineCounter);

                ihLocal.addWarning(message);
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
                ihByte.erase();
                
                ihByte = *ihLineIterator;
                ++ihLineIterator;
                ihByte += *ihLineIterator;
                ++ihLineIterator;
                
                byteRead = ihLocal.stringToHex(ihByte);
                           
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
                loadOffset = 0;
                
                /* Get the record length                                      */
                ihByte.erase();
                ihByte = *ihLineIterator;
                ++ihLineIterator;
                ihByte += *ihLineIterator;
                ++ihLineIterator;
                recordLength = ihLocal.stringToHex(ihByte);
                
                /* Get the load offset (2 bytes)                              */
                ihByte.erase();
                ihByte = *ihLineIterator;
                ++ihLineIterator;
                ihByte += *ihLineIterator;
                ++ihLineIterator;
                loadOffset = 
                        static_cast<unsigned long>(ihLocal.stringToHex(ihByte));
                loadOffset <<= 8;
                ihByte.erase();
                ihByte = *ihLineIterator;
                ++ihLineIterator;
                ihByte += *ihLineIterator;
                ++ihLineIterator;
                loadOffset += 
                        static_cast<unsigned long>(ihLocal.stringToHex(ihByte));
                
                /* Get the record type                                        */
                ihByte.erase();
                ihByte = *ihLineIterator;
                ++ihLineIterator;
                ihByte += *ihLineIterator;
                ++ihLineIterator;
                recordType = 
                   static_cast<intelhexRecordType>(ihLocal.stringToHex(ihByte));
                           
                /* Decode the INFO or DATA portion of the record              */
                switch (recordType)
                {
                    case DATA_RECORD:
                        ihLocal.decodeDataRecord(recordLength, loadOffset,
                                                 ihLineIterator);
                        cout << "Data Record" << endl;
                        break;
                    
                    case END_OF_FILE_RECORD:
                        /* Note the line number at this point. Later, check   */
                        /* that there were no more lines after this one.      */
                        /* Generate error if there were                       */
                        cout << "End of File" << endl;
                        break;
                        
                    case EXTENDED_SEGMENT_ADDRESS:
                        /* Make sure we have 2 bytes of data                  */
                        if (recordLength == 2)
                        {
                            /* Extract the two bytes of the ESA               */
                            unsigned long extSegAddress = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            extSegAddress = static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            extSegAddress <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            extSegAddress += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            
                            /* ESA is bits 4-19 of the segment base address   */
                            /* (SBA), so shift left 4 bits                    */
                            extSegAddress <<= 4;
                            
                            /* Update the SBA                                 */
                            ihLocal.segmentBaseAddress = extSegAddress;
                        }
                        else
                        {
                            /* Note the error                                 */
                            //cout << "Error in Ext. Seg. Address" << endl;
                            string message;
                            
                            message = "Extended Segment Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 2 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        cout << "Ext. Seg. Address" << endl;
                        break;
                        
                    case START_SEGMENT_ADDRESS:
                        /* Make sure we have 4 bytes of data                  */
                        if (recordLength == 4)
                        {
                            ihLocal.csRegister = 0;
                            ihLocal.ipRegister = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.csRegister = static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.csRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.csRegister += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                                
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.ipRegister = static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.ipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.ipRegister += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                        }
                        else
                        {
                            /* Note the error                                 */
                            //cout << "Error in Start Seg. Address" << endl;
                            
                            string message;
                            
                            message = "Start Segment Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 4 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        cout << "Start Seg. Address" << endl;
                        break;
                        
                    case EXTENDED_LINEAR_ADDRESS:
                        /* Make sure we have 2 bytes of data                  */
                        if (recordLength == 2)
                        {
                            /* Extract the two bytes of the ELA               */
                            unsigned long extLinAddress = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            extLinAddress = static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            extLinAddress <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            extLinAddress += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            
                            /* ELA is bits 16-31 of the segment base address  */
                            /* (SBA), so shift left 16 bits                   */
                            extLinAddress <<= 16;
                            
                            /* Update the SBA                                 */
                            ihLocal.segmentBaseAddress = extLinAddress;
                        }
                        else
                        {
                            /* Note the error                                 */
                            //cout << "Error in Ext. Lin. Address" << endl;
                            
                            string message;
                            
                            message = "Extended Linear Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 2 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        cout << "Ext. Lin. Address" << endl;
                        break;
                        
                    case START_LINEAR_ADDRESS:
                        /* Make sure we have 4 bytes of data                  */
                        if (recordLength == 4)
                        {
                            /* Extract the four bytes of the SLA              */
                            ihLocal.eipRegister = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.eipRegister = static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.eipRegister += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.eipRegister += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.eipRegister += static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                        }
                        else
                        {
                            /* Note the error                                 */
                            //cout << "Error in Start Lin. Address" << endl;
                            
                            string message;
                            
                            message = "Start Linear Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 4 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        cout << "Start Lin. Address" << endl;
                        break;
                    
                    default:
                        /* Handle the error here                              */
                        cout << "Unknown Record" << endl;
                        
                        string message;
                            
                        message = "Unknown Intel HEX record @ line " +
                                  ihLocal.ulToString(lineCounter);
                        
                        ihLocal.addError(message);
                        
                        break;
                }
            }
            else
            {
                /* Add some error code here                                   */
                //cerr << "Checksum error on line " << lineCounter << endl;
                
                string message;
                            
                message = "Checksum error @ line " + 
                          ihLocal.ulToString(lineCounter) + 
                          "; calculated 0x" + 
                          ihLocal.ucToHexString(intelHexChecksum - byteRead) +
                          " expected 0x" + 
                          ihLocal.ucToHexString(byteRead);
                
                ihLocal.addError(message);
            }
        }
    } while (ihLine.length() > 0);
    
    cout << ihLine << endl << lineCounter << endl;
    
    return(dataIn);
}

