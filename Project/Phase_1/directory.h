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

// Use below fstreams globally to open files within the functions,
// for example, ofs.open(".ifile", std::ios::binary | std::ios::out | std::ios::app); or
// ifs.open(".ifile", std::ifstream::in);
std::ofstream ofs;
std::ifstream ifs;

extern list<Inode> ifile;   // In memory ifile holder that can be acces across the program
extern Inode RootInode;

typedef struct DirMap
{
    std::string name;
    u_int inum;
} DirMap;

class Directory
{
    std::ofstream directory; // A directory file to hold the "dir" map
    // std::list<Inode> ifile;

    public:
        Directory() {}; // Defualt constructor
        void Directory_initialization();
        void Directory_create();
        void Directory_read(u_int inum, void* buffer, u_int offset, u_int length);
        void Directory_write(u_int inum, void* buffer, u_int offset, u_int length);
        void Directory_Free();

        int Directory_file_create(const char* path, std::string filename, u_int filesize, mode_t mode, mode_t type, u_int inum);
        int Directory_file_write(u_int inum, void* buffer);
        int Directory_file_read(const char* path, mode_t mode, uid_t owner, gid_t group);
        int Directory_file_free(const char* path, mode_t mode, uid_t owner, gid_t group);
};

#endif
