/*
 *  Filename: directory.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Author: Terrence Lim
 *
 *  Summary: This is a implementation of the directory layer.
 *  Directory layer calls functions from the file layer in order to create,
 *  remove, open, read, and write, etc files, which includes directory file
 *  itself. Directory is actually a special file, which holds an array of
 *  <inum, name> pairs of inodes of the files stored in the current directory.
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <iostream>

#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libgen.h>

#include "file.h"
#include "inode.h"

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
const int INUMOFIFILE = sizeof(Inode::Container);

class Directory
{
    public:
        Directory() {}; // Defualt constructor
        int Directory_initialization();
        int Directory_create(const char* path, const char* dirname, mode_t mode, mode_t type, u_int inum);    // Creates a new directory (special type) file
        int Directory_read(const char* path, int length, void* buffer);                  // Reads contents of directory file
        int Directory_write(const char* path, void* buffer, u_int offset, u_int length);                            // Writes a new file(s) into directory file
        int Directory_free(const char* path);                                                              // 

        int Directory_file_create(const char* path, const char* filename, u_int filesize, int mode, int type, u_int inum);
        int Directory_file_write(const char* path, void* buffer, u_int offset, u_int length);
        int Directory_file_read(const char* path, void* buffer, u_int offset, u_int length);
        int Directory_file_free(const char* path);
};

#endif
