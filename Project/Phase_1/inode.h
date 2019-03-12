/*
 *  Filename: inode.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
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

// C++ std. libraries list here:
#include <string>
#include <fstream>

// C std. libraries list under here:
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

// Class header files list under here:
#include "flash.h"

class Inode
{
    char*  m_file;    // File name
    mode_t m_mode; // Mode (or permission) of a file or directory
    uid_t  m_owner; // Set the defualt to the current user id
    gid_t  m_group; // Set the default to same as the owner's username

    u_int  m_filesize;
    u_int  m_inum;

    u_int* m_direct_pointer[4]; // 4 direct pointers to the first 4 block of the of the file
    // u_int* inm_dp; // Indirect pointer to a block of direct pointer Phase 2

    mode_t m_type;  // Type of an Inode. Either a regular file (S_IFREG) or directory (S_IFDIR)
    time_t last_modified;   // Holds the last modified time

    public:
        // A Constructor for inode with the passed metadata and allocate the inode in the ifile
        void Inode_Initialization(char* file, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type);

        // Update inode direct pointers using the log address
        void Update_Direct_Ptr (u_int inum, LogAddress logAddr);

        // Read an inode and returns the pointer of the inode
        Inode* Get_Inode(u_int inum, u_int offset);

        // Getter functinos for inode
        u_int Get_Filename(Inode* inode) { return inode->m_filename; }
}

#endif
