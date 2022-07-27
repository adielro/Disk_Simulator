#include "fsDisk.h"


using namespace std;

// ------------------------------------------------------------------------

/*
 * Constructor
 */
fsDisk::fsDisk() {
    sim_disk_fd = fopen(DISK_SIM_FILE , "w+");
    assert(sim_disk_fd);

    for (int i=0; i < DISK_SIZE ; i++) {
        int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
        ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
        assert(ret_val == 1);
    }

    fflush(sim_disk_fd);
    is_formated = false;
    BitVectorSize = 0;
    block_size = 0;
    free_blocks = 0;
    BitVectorSize = 0;
}
/*
 * Destructor - iterating through MainDir, deleting FsFile and FileDescriptor
 * clearing MainDir and OpenFileDescriptors, deleting BitVector and closing sim_disk_fd (file)
 */
fsDisk::~fsDisk() {
    for (int i = 0; i < MainDir.size(); i++) {
        delete MainDir[i]->getFsFile();
        delete MainDir[i];
    }
    MainDir.clear();
    OpenFileDescriptors.clear();
    delete[] BitVector;
    fclose(sim_disk_fd);
}

// ------------------------------------------------------------------------
/*
 * Printing the disk memory
 */
void fsDisk::listAll() {
    int i = 0;

    for (i=0; i < MainDir.size(); i++) {
        cout << "index: " << i << ": FileName: " << MainDir[i]->getFileName()  <<  " , isInUse: " << MainDir[i]->getInUse() << endl;
    }
    char bufy;
    cout << "Disk content: '";
    for (i = 0; i < DISK_SIZE; i++)
    {
        cout << "(";
        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
        ret_val = fread(&bufy, 1, 1, sim_disk_fd);
        cout << bufy;
        cout << ")";
    }
    cout << "'" << endl;
}



// ------------------------------------------------------------------------
/*
 * Format the disk
 * function receive block size and initialize BitVector with zero's (BitVectorSize = disk size / block size)
 * iterating MainDir, deleting FsFile and FileDescriptors, clearing MainDir and OpenFileDescriptors
 * deleting the disk memory (write "\0" DISK_SIZE times to the file)
 */
void fsDisk::fsFormat(int blockSize) {
    BitVectorSize = floor(DISK_SIZE / blockSize);
    delete[] BitVector;
    BitVector = new int[BitVectorSize];
    for (int i = 0; i < BitVectorSize; i++)
        BitVector[i] = 0;
    this->block_size = blockSize;
    free_blocks = BitVectorSize;
    for (int i = 0; i < MainDir.size(); i++) {
        delete MainDir[i]->getFsFile();
        delete MainDir[i];
    }
    MainDir.clear();
    OpenFileDescriptors.clear();
    assert(sim_disk_fd);
    for (int i = 0; i < DISK_SIZE ; i++) {
        int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
        ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
        assert(ret_val == 1);
    }
    fflush(sim_disk_fd);
    is_formated = true;
    cout<< "FORMAT DISK: number of blocks: " << BitVectorSize << endl;
}
/*
 * The function receives a file name and returns the MainDir[FileDescriptor] index if the file exists,else return -1
 */
int fsDisk::doesFileExist(string fileName){
    int i;
    for (i = 0; i < MainDir.size(); i++)
        if (MainDir[i]->getFileName() == fileName)
            break;
    if (i == MainDir.size()){
        cout<< "File does not exist." << endl;
        return -1;
    }
    return i;
}
// ------------------------------------------------------------------------
/*
 * The function receives a file name and tries to create a new file (and open it on success)
 * first, check if: the disk is formatted, existing file with the same file name
 * create a new FsFile and FileDescriptor (link them: FileDescriptor->FsFile = FsFile)
 * insert FileDescriptor into MainDir, insert pair (FD index, FileDescriptor) into OpenFileDescriptors
 * decrement the free blocks of the disk by one
 * return the OpenFileDescriptors key (int) on success, else return -1
 */
int fsDisk::CreateFile(string fileName) {
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return -1;
    }
    for (int i = 0; i < MainDir.size(); i++)
        if (MainDir[i]->getFileName() == fileName){
            cout<< "File name already exist." << endl;
            return -1;
        }
    FsFile *create_Fs = new FsFile(this->block_size);
    FileDescriptor *fd = new FileDescriptor(fileName, create_Fs);
    MainDir.push_back(fd);
    int i = 0;
    while(1) {
        if (OpenFileDescriptors.find(i) == OpenFileDescriptors.end()) {
            OpenFileDescriptors.insert({i, fd});
            return i;
        }
        i++;
    }
}

// ------------------------------------------------------------------------
/*
 * The function receives a file name and tries to open it
 * first, check if: the disk is formatted, the file exists (return -1 on false)
 * check if the file already open- iterating through OpenFileDescriptors searching for the file (return -1 if found)
 * searching for a free FD number in OpenFileDescriptors and insert the FileDescriptor with the FD number as a key
 * return the FD number on success
 */
int fsDisk::OpenFile(string fileName) {
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return -1;
    }
    int i = doesFileExist(fileName);
    if (i == -1)
        return -1;
    for (auto entry : OpenFileDescriptors)
        if (entry.second->getFileName() == fileName){
            cout<< "File already open." << endl;
            return -1;
        }
    MainDir[i]->setInUse(true);
    for (int ind = 0; ind < BitVectorSize; ind++)
        if (OpenFileDescriptors.find(ind) == OpenFileDescriptors.end()) {
            OpenFileDescriptors.insert({ind, MainDir[i]});
            return ind;
        }
    return -1;
}

// ------------------------------------------------------------------------
/*
 * The function receives a file FD number and tries to close the requested file
 * first, check if the disk is formatted (return -1 on false)
 * check if the file is open (if the FD number is found in OpenFileDescriptors) and return -1 on false
 * removing the FileDescriptor from OpenFileDescriptors and returning the file name
 */
string fsDisk::CloseFile(int fd) {
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return "-1";
    }
    if (OpenFileDescriptors.find(fd) != OpenFileDescriptors.end()){
        OpenFileDescriptors.find(fd)->second->setInUse(false);
        string deletedFileName = OpenFileDescriptors.find(fd)->second->getFileName();
        OpenFileDescriptors.erase(fd);
        return deletedFileName;
    }
    cout << "File already closed or does not exist." << endl;
    return "-1";
}
// ------------------------------------------------------------------------
/*
 * The function receives a FileDescriptor number, string, and string length, trying to write it to the file
 * first check: disk is formatted, the file is open, enough memory in disk and file (return -1 on false)
 * if no block index is occupied: iterate through BitVector searching for free block (return -1 if disk is full)
 * writing to file algorithm:
 * (1) as long there are chars to write keep going, else we are done and return
 * (2) check if there is free memory in the file, skip to (4) if true
 * (3) iterate through BitVector to find a free block, occupy it and write it to the block index
 * (4) check the last index on the block index, navigate to the index and write the char
 */
int fsDisk::WriteToFile(int fd, char *buf, int len ) {
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return -1;
    }
    auto search = OpenFileDescriptors.find(fd);
    if (search == OpenFileDescriptors.end()){
        cout << "File not open." << endl;
        return -1;
    }
    FsFile* current_file = search->second->getFsFile();
    int indexBlockNeeded = 0;
    if (current_file->getIndexBlock() == -1)
        indexBlockNeeded = 1;
    int remaining_file_memory = current_file->remaining_memory();
    if (remaining_file_memory - len <= 0)
        if ((free_blocks*block_size) + remaining_file_memory - len - (indexBlockNeeded * block_size) < 0 || current_file->getFile_size() + len > (block_size * block_size)){
            cout << "Not enough memory" << endl;
            return -1;
        }
    if (indexBlockNeeded == 1) {
        int block_ind;
        for (block_ind = 0; block_ind < BitVectorSize; block_ind++)
            if (BitVector[block_ind] == 0){
                BitVector[block_ind] = 1;
                current_file->setIndexBlock(block_ind);
                free_blocks--;
                break;
            }
    }
    int c;
    int runner = 0;
    while (len > 0) {
        if (current_file->remaining_memory() == 0){
            for (int i = 0; i < BitVectorSize; i++)
                if (BitVector[i] == 0){
                    current_file->addBlockInUse();
                    BitVector[i] = 1;
                    free_blocks--;
                    fseek(sim_disk_fd, current_file->getIndexBlock()*block_size+current_file->getBlockInUse() - 1, SEEK_SET);
                    putc(i + ASCII_DIFF, sim_disk_fd);
                    break;
                }
        }
        fseek(sim_disk_fd, current_file->getIndexBlock()*block_size+current_file->getBlockInUse() - 1, SEEK_SET);
        c = fgetc(sim_disk_fd) - ASCII_DIFF;
        fseek(sim_disk_fd, c * block_size + block_size - current_file->remaining_memory(), SEEK_SET);
        while (current_file->remaining_memory() > 0){
            putc(buf[runner++], sim_disk_fd);
            current_file->writeChar();
            len--;
            if (len == 0)
                break;
        }
    }
    return fd;
}
// ------------------------------------------------------------------------
/*
 * The function receives a file name and tries to delete it
 * first check: disk is formatted, the file exists (return -1 on false)
 * if the file is open remove it from OpenFileDescriptors
 * deleting file memory algorithm:
 * (1) as long there are blocks_in_use keep going, else we are done, delete the index block, free BitVector[i]
 * (2) navigate to the file block index, locate the data block index and navigate to it
 * (3) delete the data index, decrement block_in_use by one, and free BitVector[i]
 * delete the FsFile and FileDescriptor, remove FileDescriptor from MainDir and OpenFileDescriptors (if was open)
 */
int fsDisk::DelFile(string FileName) {
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return -1;
    }
    int i;
    for (i = 0; i < MainDir.size(); i++)
        if (MainDir[i]->getFileName() == FileName)
            break;
    if (i == MainDir.size()){
        cout << "File doesn't exist." << endl;
        return -1;
    }
    if (MainDir[i]->getInUse() == 1)
        for (auto entry : OpenFileDescriptors)
            if (entry.second->getFileName() == FileName){
                CloseFile(entry.first);
                break;
            }
    int counter = 0;
    while (MainDir[i]->getFsFile()->getBlockInUse() > 0){
        fseek(sim_disk_fd, MainDir[i]->getFsFile()->getIndexBlock()*block_size + counter, SEEK_SET);
        int c = fgetc(sim_disk_fd) - ASCII_DIFF;
        fseek(sim_disk_fd, (c * block_size), SEEK_SET);
        for (int j = 0; j < block_size; j++) {
            int ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        MainDir[i]->getFsFile()->deleteChar(block_size);
        BitVector[c] = 0;
        free_blocks++;
        MainDir[i]->getFsFile()->subBlockInUse();
        counter++;
    }
    fseek(sim_disk_fd, MainDir[i]->getFsFile()->getIndexBlock() * block_size,
          SEEK_SET);
    for (int j = 0; j < block_size; j++) {
        int ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
        assert(ret_val == 1);
    }
    BitVector[MainDir[i]->getFsFile()->getIndexBlock()] = 0;
    free_blocks++;
    delete MainDir[i]->getFsFile();
    delete MainDir[i];
    MainDir.erase(MainDir.begin() + i);
    return i;
}
// ------------------------------------------------------------------------
/*
 * The function receives a FileDescriptor number, string, and the string length, trying to fill it with the file data
 * first check: disk is formatted, file is open, enough data to read (file_size >= length), return -1 on false
 * read from file algorithm:
 * (1) initialize counter = 0, while (counter < length) keep going
 * (2) navigate to the file index block, locate the data index block and navigate to it
 * (3) insert the requested char into the string, increment counter by one
 */
int fsDisk::ReadFromFile(int fd, char *buf, int len) {
    buf[0] = '\0';
    if (!this->is_formated){
        cout << "DISK not formatted" << endl;
        return -1;
    }
    auto search = OpenFileDescriptors.find(fd);
    if (search == OpenFileDescriptors.end()){
        cout << "File not open." << endl;
        return -1;
    }
    FsFile* current_file = search->second->getFsFile();
    if (len > current_file->getFile_size()){
        cout << "Not enough data to read." << endl;
        return -1;
    }
    int counter = 0;
    while (counter < len){
        fseek(sim_disk_fd, current_file->getIndexBlock()*block_size + (counter / block_size), SEEK_SET);
        int c = fgetc(sim_disk_fd) - ASCII_DIFF;
        fseek(sim_disk_fd, (c * block_size) + (counter % block_size), SEEK_SET);
        buf[counter++] = fgetc(sim_disk_fd);
        buf[counter] = '\0';
    }
    return fd;
}
