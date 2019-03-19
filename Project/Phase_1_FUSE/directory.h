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

typedef struct DirMap
{
    std::string name;
    u_int inum;
} DirMap;

// Extern variables
extern std::list<Inode> ifile;      // In memory ifile holder that can be acces across the program
extern std::list<DirMap> directory; // This holds the in-memory directory <name,inum> list
extern Inode inode_of_ifile;        // This holds the inode of ifile

const int INUMOFROOTDIR = 2;        // Following the UNIX system's tradition of root directory's inum is 2, fixxing the root inum to 2
const int INUMOFIFILE = 5;          // Fix the ifile's inum to 5

class Directory
{
    public:
        Directory() {}; // Defualt constructor
        int Directory_initialization();
        int Directory_create(std::string path, std::string dirname, int mode, int type, u_int inum);
        int Directory_read(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_write(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_Free(u_int inum);

        int Directory_file_create(std::string path, std::string filename, u_int filesize, int mode, int type, u_int inum);
        int Directory_file_write(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_file_read(u_int inum, void* buffer, u_int offset, u_int length);
        int Directory_file_free(u_int inum);
};

#endif
