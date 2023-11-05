Intel HEX Class Project
=======================

Introduction to the Intel HEX Class Module
------------------------------------------
The Intel HEX Class Module is designed to allow the programmer to easily work with Intel HEX files. Using standard C++ stream operators it is possible to decode a file into memory and then to examine the entire content of the file much more easily than if the original file was simply opened in a text editor.

Additionally, again using a C++ stream operator, it is possible to encode the current data in memory back into Intel HEX format again straight to a file.

Using this class it is also possible to:

* Generate Intel HEX files with your own content
* Fill blank areas of an Intel HEX file with a desired value or pattern
* Merge two or more files together
* Selectively merge parts of two or more files together
* Check an Intel HEX file for correctness and integrity (correct construction, checksums ok, etc.)

Key Software Capabilities
-------------------------
* Stream Intel HEX file content into class for decoding
* Stream data stored in memory out from class in Intel HEX format to a file
* 'Never give up' approach to decoding; warning and error messages can be examined upon completion; as much as humanely possible will have been decoded.
* Class contains no 'try' calls; will always return cleanly for programmer to make decision how to proceed.
* File sizes up to 4GB supported (system dependent).
* All records as per the official Intel "Hexadecimal Object File Format Specification, Revision A, January 6, 1988" supported
