/*
 *  Filename: file.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: The file layer is responsible for implementing the file abstraction. A file is represented by
 *  an inode containing the metadata for the file, including the file type (e.g. file or
 *  directory), size, and the flash addresses of the file's blocks.
 */

#ifndef FILE_H
#define FILE_H

// C++ std. libraries list here:
#include <fstream>
#include <string>

// C std. libraries list under here:
#include <time.h>
#include <math.h>

// Class header files list under here:
#include "inode.h"
#include "log.h"

extern u_int BLOCK_SIZE;    // This block size should be available across the program. Initialization should be done in LFS.cpp

class File
{
    public:
        File() {};  // Default constructor
        int File_Create(Inode* inode, const char* path, const char* filename, u_int inum, u_int filesize, mode_t mode, mode_t type);
        int File_Write(u_int inum, u_int offset, u_int length, void* buffer);
        int File_Read(u_int inum, u_int offset, u_int length, void* buffer);
        int File_Free(u_int inum);
};

#endif
