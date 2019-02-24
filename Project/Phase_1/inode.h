/*
 *  Filename: inode.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 2/24/2019
 *  Author: Terrence Lim
 *  Summary: Implements inode of a file system, which holds attributes of a file.
 *  Inode gets created when a new file gets created, then stored in the log.
 *  When, the file gets updated, associated inode also gets updated and stored in
 *  the different location of the log.
 *  Inode holds information of a file, such as the filename, permission, owner,
 *  group (default is same as the owner), filesize, inumber, type (directory/file),
 *  4 direct pointers to the first 4 blocks of the file and an indirect pointer
 *  to a block of direct pointers.
 */

#ifndef INODE_H
#define INODE_H

#include <string>
#include <fstream>

#include <flash.h>

class Inode
{
    std::string filename;
    std::string permission;
    std::string owner;
    std::string group; // Default is same as the owner's username

    u_int filesize;
    u_int inum;

    u_int* dp_1;  // 4 direct pointers to the first 4 block of the of the file
    u_int* dp_2;
    u_int* dp_3;
    u_int* dp_4;
    u_int* indp; // Indirect pointer to a block of direct pointers

    char type; // "D" for directory or "F" for file

    std::ofstream ifile; // All inodes must be stored in the "ifile"

    public:
        // Create a new inode with the passed metadata and allocate the inode in the ifile
        void Inode_Create(std::string filename, std::string permission, u_int filesize, u_int inum, char type,
                          u_int* dp_1, u_int* dp_2, u_int* dp3, u_int* dp4, u_int* indp);
        // Read an inode and returns the pointer of the inode
        Inode* Inode_read(u_int inum, u_int offset);
        // When a file gets updated, update the inode and place it in the new location
        void Inode_update(Inode* inode);
}

#endif
