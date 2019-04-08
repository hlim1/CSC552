/*
 *  Filename: directory.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: 
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <sys/stat.h>

#include "file.h"
#include "Inode.h"

typedef struct DirMap
{
    char name[20];
    u_int inum;
} DirMap;

// Extern variables
extern std::list<DirMap> directory; // This holds the in-memory directory <name,inum> list
extern Inode inode_of_ifile;        // This holds the inode of ifile
extern Inode inode_of_current_file; // This inode holds the inode of currently accessing file

const int INUMOFROOTDIR = 0;        // Inum of the root directory, which is the very first file that gets created get a deafuly inum of 0
const int INUMOFIFILE = sizeof(Inode::Container);          // Fix the ifile's inum to 5

class Directory
{
    public:
        Directory() {}; // Defualt constructor
        int Directory_initialization();
        int Directory_create(const char* path, const char* dirname, int mode, int type, u_int inum);    // Creates a new directory (special type) file
        int Directory_read(const char* path, const char* dirname, void* buffer);                                             // Reads contents of directory file
        int Directory_write(u_int inum, void* buffer, u_int offset, u_int length);                      // Writes a new file(s) into directory file
        int Directory_Free(u_int inum);                                                                 // 

        int Directory_file_create(const char* path, const char* filename, u_int filesize, int mode, int type, u_int inum);
        int Directory_file_write(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_file_read(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_file_free(u_int inum);
};

#endif
