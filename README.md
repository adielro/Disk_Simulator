# Disk_Simulator
A simulator of how the disk memory works
==Description==

In this program, we are simulating how the memory on the disk works and how it is managed by the system.


Functions:

ListAll:
Printing the disk memory

fsFormat:
Format the disk
the function receives a block size and initialize BitVector with zero's (BitVectorSize = disk size / block size)
iterating MainDir, deleting FsFile and FileDescriptors, clearing MainDir and OpenFileDescriptors
deleting the disk memory (write "\0" DISK_SIZE times to the file)

doesFileExist:
The function receives a file name and returns the MainDir[FileDescriptor] index if the file exists, else return -1

CreateFile:
The function receives a file name and tries to create a new file (and open it on success)
first, check if: the disk is formatted, an existing file with the same file name
iterate through BitVector searching for free block (return -1 if disk is full), BitVector[i] = 1 if found
create a new FsFile and FileDescriptor (link them: FileDescriptor->FsFile = FsFile)
insert FileDescriptor into MainDir, insert pair (FD index, FileDescriptor) into OpenFileDescriptors
decrement the free blocks of the disk by one
return the OpenFileDescriptors key (int) on success, else return -1

OpenFile:
The function receives a file name and tries to open it
first, check if: the disk is formatted, the file exists (return -1 on false)
check if the file is already open - iterating through OpenFileDescriptors searching for the file (return -1 if found)
searching for a free FD number in OpenFileDescriptors and insert the FileDescriptor with the FD number as a key
return the FD number on success

CloseFile:
The function receives a file FD number and tries to close the requested file
first, check if the disk is formatted (return -1 on false)
check if the file is open (if the FD number is found in OpenFileDescriptors) and return -1 on false
removing the FileDescriptor from OpenFileDescriptors and returning the file name

WriteToFile:
The function receives a FileDescriptor number, string, and string length, trying to write it to the file
first check: disk is formatted, the file is open, enough memory in disk and file (return -1 on false)
writing to file algorithm:
(1) as long there are chars to write keep going, else we are done and return
(2) check if there is free memory in the file, skip to (4) if true
(3) iterate through BitVector to find a free block, occupy it and write it to the block index
(4) check the last index on the block index, navigate to the index and write the char

DelFile:
The function receives a file name and tries to delete it
first check: disk is formatted, the file exists (return -1 on false)
if the file is open remove it from OpenFileDescriptors
deleting file memory algorithm:
(1) as long there are blocks_in_use keep going, else we are done, delete the index block, free BitVector[i]
(2) navigate to the file block index, locate the data block index and navigate to it
(3) delete the data index, decrement block_in_use by one, and free BitVector[i]
delete the FsFile and FileDescriptor, remove FileDescriptor from MainDir and OpenFileDescriptors (if was open)

ReadFromFile:
The function receives a FileDescriptor number, string, and the string length, trying to fill it with the file data
first check: disk is formatted, file is open, enough data to read (file_size >= length), return -1 on false
read from file algorithm:
(1) initialize counter = 0, 
while (counter < length) keep going
(2) navigate to the file index block, locate the data index block and navigate to it
(3) insert the requested char into the string, increment counter by one




==Program files==

FsFile.h
FileDescriptor.h
fsDisk.h
fsDisk.cpp
main.cpp


==How to complie==

compile: g++ main.cpp fsDisk.cpp -o simulation
run: ./simulation
