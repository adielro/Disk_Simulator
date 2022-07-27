#ifndef FSFILE_H
#define FSFILE_H

class FsFile {
// ============================================================================
/*
 * FsFile - represent created files
 */
        int file_size;
        int block_in_use;
        int index_block;
        int block_size;

    public:
        FsFile(int _block_size) {
            file_size = 0;
            block_in_use = 0;
            block_size = _block_size;
            index_block = -1;
        }

        int getFile_size(){
            return file_size;
        }
        void writeChar(){
            file_size++;
        }

        void deleteChar(int amount){
            file_size -= amount;
        }

        int getIndexBlock(){
            return index_block;
        }

        int getBlockInUse(){
            return block_in_use;
        }

        void addBlockInUse(){
            block_in_use++;
        }

        void subBlockInUse(){
            block_in_use--;
        }

        void setIndexBlock(int ind){
            this->index_block = ind;
        }
/* Function return the remaining memory including only the allocated blocks */
        int remaining_memory(){
            return (block_in_use*block_size) - file_size;
        }
};
#endif
