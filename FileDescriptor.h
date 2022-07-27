//
// Created by Adiel on 27/07/2022.
//

#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H
#include "FsFile.h"
#include <string>

using namespace std;

class FileDescriptor {
// ============================================================================
/*
 * FileDescriptor for each file
 */
        string file_name;
        FsFile* fs_file;
        bool inUse;

    public:
        FileDescriptor(string FileName, FsFile* fsi) {
            file_name = FileName;
            fs_file = fsi;
            inUse = true;
        }

        string getFileName() {
            return file_name;
        }

        bool getInUse(){
            return inUse;
        }

        void setInUse(bool interaction){
            this->inUse = interaction;
        }

        FsFile* getFsFile(){
            return this->fs_file;
        }
};


#endif
