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

class Directory
{
    std::map<std::string, u_int> dir; // A map of <name, inum>
    std::ofstream directory; // A directory file to hold the "dir" map

    public:
        void Directory_create (std::string file, u_int inum);
}

#endif
