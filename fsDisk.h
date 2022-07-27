/*
 * Disk Memory Simulator
 * Authored by Adiel Rozenfeld - 208349019
 */

#ifndef FSDISK_H
#define FSDISK_H
#include "FileDescriptor.h"
#include "FsFile.h"
#include <vector>
#include <map>
#include <cassert>
#include <string>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <cstring>

#define DISK_SIZE 256
#define ASCII_DIFF 48
#define DISK_SIM_FILE "DISK_SIM_FILE.txt"

class fsDisk {
    FILE *sim_disk_fd;
    bool is_formated;
    int BitVectorSize;
    int *BitVector;
    int block_size;
    int free_blocks;
    vector<FileDescriptor*> MainDir;
    map<int, FileDescriptor*> OpenFileDescriptors;

public:
    fsDisk();
    ~fsDisk();
    void listAll();
    void fsFormat(int blockSize = 4);
    int doesFileExist(string fileName);
    int CreateFile(string fileName);
    int OpenFile(string fileName);
    string CloseFile(int fd);
    int WriteToFile(int fd, char *buf, int len );
    int DelFile(string FileName);
    int ReadFromFile(int fd, char *buf, int len);


};


#endif
