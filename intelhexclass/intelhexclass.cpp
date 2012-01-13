#include <iostream>
#include <string>
#include <cstdio>

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
        valueIterator = value.begin();
        
        for (int x=0; x < 2; x++)
        {
            /* Shift result variable 4 bits to the left                       */
            returnValue <<= 4;
            
            if (*valueIterator >= '0' && *valueIterator <= '9')
            {
                returnValue += 
                               static_cast<unsigned char>(*valueIterator - '0');
            }
            else if (*valueIterator >= 'A' && *valueIterator <= 'F')
            {                
                returnValue += 
                          static_cast<unsigned char>(*valueIterator - 'A' + 10);
            }
            else if (*valueIterator >= 'a' && *valueIterator <= 'f')
            {
                returnValue += 
                          static_cast<unsigned char>(*valueIterator - 'a' + 10);
            }
            else
            {
                /* Error occured - non-HEX value found                        */
                string message;

                message = "Can't convert byte 0x" + value + " @ 0x" + 
                          ulToHexString(segmentBaseAddress) + " to hex.";

                addError(message);
                
                returnValue = 0;
            }

            /* Iterate to next char in the string                             */
            ++valueIterator;
        }
    }
    else
    {
        /* Error occured - more or less than two nibbles in the string        */
        string message;

        message = value + " @ 0x" + ulToHexString(segmentBaseAddress) + 
                                                       " isn't an 8-bit value.";

        addError(message);
    }
    
    return returnValue;
}

/* Converts an unsigned long char to a string in HEX format                   */
string intelhex::ulToHexString(unsigned long value)
{
    string returnString;
    char localString[50];
    
    returnString.erase();
    
    snprintf(localString, 49, "%08lX", value);
    
    returnString.insert(0, localString);

    return returnString;
}

string intelhex::ulToString(unsigned long value)
{
    string returnString;
    char localString[50];
    
    returnString.erase();
    
    snprintf(localString, 49, "%lu", value);
    
    returnString.insert(0, localString);

    return returnString;
}

string intelhex::ucToHexString(unsigned char value)
{
    string returnString;
    char localString[50];
    
    returnString.erase();
    
    snprintf(localString, 49, "%02X", value);
    
    returnString.insert(0, localString);

    return returnString;
}

void intelhex::addWarning(string warningMessage)
{
    string localMessage;
    
    /* Build the message and push the warning message onto the list           */
    localMessage += ulToString(msgWarning.noOfWarnings + 1) + " Warning: " 
                                                               + warningMessage;
    
    msgWarning.ihWarnings.push_back(localMessage);
    
    /* Update the number of warning messages                                  */
    msgWarning.noOfWarnings = msgWarning.ihWarnings.size();
}

void intelhex::addError(string errorMessage)
{
    string localMessage;
    
    /* Build the message and push the error message onto the list             */
    localMessage += ulToString(msgError.noOfErrors + 1) + " Error: " 
                                                                 + errorMessage;
    
    msgError.ihErrors.push_back(localMessage);
    
    /* Update the number of error messages                                    */
    msgError.noOfErrors = msgError.ihErrors.size();
}

void intelhex::decodeDataRecord(unsigned char recordLength,
                                unsigned long loadOffset,
                                string::const_iterator data)
{
    /* Variable to store a byte of the record as a two char string            */
    string sByteRead;
    
    /* Variable to store the byte of the record as an u.char                  */
    unsigned char byteRead;
    
    /* Calculate new SBA by clearing the low four bytes and then adding the   */
    /* current loadOffset for this line of Intel HEX data                     */
    segmentBaseAddress &= ~(0xFFFFUL);
    segmentBaseAddress += loadOffset;
    
    for (unsigned char x = 0; x < recordLength; x ++)
    {
        sByteRead.erase();
        
        sByteRead = *data;
        data++;
        sByteRead += *data;
        data++;

        byteRead = stringToHex(sByteRead);
        
        ihReturn=ihContent.insert(
                         pair<int,unsigned char>(segmentBaseAddress, byteRead)); 
        
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
        }

        /* Increment the segment base address                                 */
        ++segmentBaseAddress;
    }
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
            else
            {
                /* Remove the record mark from the string as we don't need it */
                /* anymore                                                    */
                ihLine.erase(ihLineIterator);
            }
        
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
                /* Just in case there are an odd number of chars in the       */
                /* just check we didn't reach the end of the string early     */
                if (ihLineIterator != ihLine.end())
                {
                    ihByte += *ihLineIterator;
                    ++ihLineIterator;
                
                    byteRead = ihLocal.stringToHex(ihByte);
                           
                    intelHexChecksum += byteRead;
                }
                else
                {
                    string message;
                            
                    message = "Odd number of characters in line " +
                                                ihLocal.ulToString(lineCounter);
                            
                    ihLocal.addError(message);
                }
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
                        if (ihLocal.verbose == true)
                        {
                            cout << "Data Record begining @ 0x" << 
                                      ihLocal.ulToHexString(loadOffset) << endl;
                        }
                        break;
                    
                    case END_OF_FILE_RECORD:
                        /* Note the line number at this point. Later, check   */
                        /* that there were no more lines after this one.      */
                        /* Generate error if there were                       */
                        if (ihLocal.verbose == true)
                        {
                            cout << "End of File" << endl;
                        }
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
                            string message;
                            
                            message = "Extended Segment Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 2 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        if (ihLocal.verbose == true)
                        {
                            cout << "Ext. Seg. Address found: 0x" <<
                               ihLocal.ulToHexString(ihLocal.segmentBaseAddress)
                                                                        << endl;
                        }
                        
                        break;
                        
                    case START_SEGMENT_ADDRESS:
                        /* Make sure we have 4 bytes of data, and that no     */
                        /* Start Segment Address has been found to date       */
                        if (recordLength == 4 && 
                            ihLocal.startSegmentAddress.exists == false)
                        {
                            /* Note that the Start Segment Address has been   */
                            /* found.                                         */
                            ihLocal.startSegmentAddress.exists = true;
                            /* Clear the two registers, just in case          */
                            ihLocal.startSegmentAddress.csRegister = 0;
                            ihLocal.startSegmentAddress.ipRegister = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startSegmentAddress.csRegister = 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.startSegmentAddress.csRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startSegmentAddress.csRegister += 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                                
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startSegmentAddress.ipRegister = 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.startSegmentAddress.ipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startSegmentAddress.ipRegister += 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                        }
                        else if (ihLocal.startSegmentAddress.exists == true)
                        {
                            /* Note the error                                 */
                            string message;
                            
                            message = "Start Segment Address record appears again @ line " + 
                                      ihLocal.ulToString(lineCounter) + 
                                      "; repeated record ignored.";
                            
                            ihLocal.addError(message);
                        }
                        if (recordLength != 4)
                        {
                            /* Note the error                                 */
                            string message;
                            
                            message = "Start Segment Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 4 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        if (ihLocal.verbose == true)
                        {
                            cout << "Start Seg. Address - CS 0x" <<
                                 ihLocal.ulToHexString(ihLocal.startSegmentAddress.csRegister) <<
                                 " IP 0x" << 
                                 ihLocal.ulToHexString(ihLocal.startSegmentAddress.ipRegister) 
                                                                        << endl;
                        }
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
                        if (ihLocal.verbose == true)
                        {
                            cout << "Ext. Lin. Address 0x" << 
                               ihLocal.ulToHexString(ihLocal.segmentBaseAddress)
                                                                        << endl;
                        }
                        
                        break;
                        
                    case START_LINEAR_ADDRESS:
                        /* Make sure we have 4 bytes of data                  */
                        if (recordLength == 4 && 
                                      ihLocal.startLinearAddress.exists == false)
                        {
                            /* Extract the four bytes of the SLA              */
                            ihLocal.startLinearAddress.eipRegister = 0;
                            
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startLinearAddress.eipRegister = 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.startLinearAddress.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startLinearAddress.eipRegister += 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.startLinearAddress.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startLinearAddress.eipRegister += 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                            ihLocal.startLinearAddress.eipRegister <<= 8;
                            ihByte.erase();
                            ihByte = *ihLineIterator;
                            ++ihLineIterator;
                            ihByte += *ihLineIterator;
                            ++ihLineIterator;
                            ihLocal.startLinearAddress.eipRegister += 
                            static_cast<unsigned long>
                                                  (ihLocal.stringToHex(ihByte));
                        }
                        else if (ihLocal.startLinearAddress.exists == true)
                        {
                            /* Note the error                                 */
                            string message;
                            
                            message = "Start Linear Address record appears again @ line " + 
                                      ihLocal.ulToString(lineCounter) + 
                                      "; repeated record ignored.";
                            
                            ihLocal.addError(message);
                        }
                        if (recordLength != 4)
                        {
                            /* Note the error                                 */
                            string message;
                            
                            message = "Start Linear Address @ line " +
                                      ihLocal.ulToString(lineCounter) + 
                                      " not 4 bytes as required.";
                            
                            ihLocal.addError(message);
                        }
                        if (ihLocal.verbose == true)
                        {
                            cout << "Start Lin. Address - EIP 0x" <<
                                 ihLocal.ulToHexString(ihLocal.startLinearAddress.eipRegister) 
                                                                        << endl;
                        }
                        break;
                    
                    default:
                        /* Handle the error here                              */
                        if (ihLocal.verbose == true)
                        {
                            cout << "Unknown Record @ line " << 
                                        ihLocal.ulToString(lineCounter) << endl;
                        }
                        
                        
                        string message;
                            
                        message = "Unknown Intel HEX record @ line " +
                                  ihLocal.ulToString(lineCounter);
                        
                        ihLocal.addError(message);
                        
                        break;
                }
            }
            else
            {
                /* Note that the checksum contained an error                  */
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
    
    if (ihLocal.verbose == true)
    {
        cout << "File contained " << lineCounter << " lines." << endl;
    }
    
    return(dataIn);
}

