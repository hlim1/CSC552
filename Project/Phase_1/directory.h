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

#include <sys/stat.h>

#include "file.h"

// Use below fstreams globally to open files within the functions,
// for example, ofs.open(".ifile", std::ios::binary | std::ios::out | std::ios::app); or
// ifs.open(".ifile", std::ifstream::in);
std::ofstream ofs;
std::ifstream ifs;

typedef struct DirMap
{
    std::string name;
    u_int inum;
} DirMap;

class Directory
{
    std::map<std::string, u_int> dir; // A map of <name, inum>
    std::ofstream directory; // A directory file to hold the "dir" map
    std::list<Inode> ifile;

    public:
        Directory() {};
        void Directory_initialization();
        void Directory_create();
        void Directory_read();
        void Directory_write();
        void Directory_Free();

        int Directory_file_create(const char* path, std::string file, u_int filesize, mode_t mode, mode_t type, u_int inum);
        int Directory_file_write(const char* path, std::string file, u_int inum);
        int Directory_file_read(const char* path, mode_t mode, uid_t owner, gid_t group);
        int Directory_file_free(const char* path, mode_t mode, uid_t owner, gid_t group);
};

#endif
