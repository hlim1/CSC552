/*
 *  Filename: directory.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 2/24/2019
 *  Author: Terrence Lim
 *  Summary: 
 */

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <map>
#include <string>
#include <fstream>
#include <stat.h>

class Directory
{
    std::map<std::string, u_int> dir; // A map of <name, inum>
    std::ofstream directory; // A directory file to hold the "dir" map

    public:
        void Directory_create();
        void Directory_read();
        void Directory_write();
        void Directory_Free();

        void Driectory_file_create(const char* path, mode_t mode, uid_t owner, gid_t group);
        void Driectory_file_write(const char* path, mode_t mode, uid_t owner, gid_t group);
        void Driectory_file_read(const char* path, mode_t mode, uid_t owner, gid_t group);
        void Driectory_file_free(const char* path, mode_t mode, uid_t owner, gid_t group);
}

#endif
