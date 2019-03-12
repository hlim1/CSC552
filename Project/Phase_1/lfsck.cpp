/*
 *  Filename: lfsck.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: lfsck receives a virtual flash file name as a command line
 *  and checks LFS for a consistency. lfsck will check
 *  1. in-use inodes that do not have directory entries
 *  2. directory entries that refer to unused inodes
 *  3. incorrect segment summary information
 *  It will use flash ayer to convert the LFS data structure to JSON,
 *  then processes JSON and checks for the consistency
 */

// C++ std. libraries list here:
#include <iostream>
#include <string>

// Class header files list under here:
#include "ionde.h"
#include "file.h"
#include "directory.h"

int main (int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Command not valid. Please, enter in a format of $lfsck <file_name>." << std::endl;
        return 1;
    }

    std::string file = argv[1];
    int size;
    Inode* root;
    Inode* ifile;
    Inode* inode;

    return 0;
}
