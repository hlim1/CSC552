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
#include <list>

// C std. libraries list under here:
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

// Class header files list under here:
// #include "log.h"

typedef struct Direct_Block_Ptr
{
    u_int segment;
    u_int block;
} Direct_Block_Ptr;

class Inode
{
    std::string  m_file;    // File name
    mode_t m_mode;    // Mode (or permission) of a file or directory
    uid_t  m_owner;   // Set the defualt to the current user id
    gid_t  m_group;   // Set the default to same as the owner's username

    u_int  m_filesize;
    u_int  m_inum;

    Direct_Block_Ptr m_direct_pointer[4]; // 4 direct pointers to the first 4 block of the of the file
    // u_int* inm_dp; // Indirect pointer to a block of direct pointer Phase 2

    mode_t m_type;  // Type of an Inode. Either a regular file (S_IFREG) or directory (S_IFDIR)
    time_t last_modified;   // Holds the last modified time

    public:
        Inode() {};
        // A Constructor for inode with the passed metadata and allocate the inode in the ifile
        int Inode_Initialization(std::string filename, std::string path, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type);
        int Inode_Write(Inode &found_inode, u_int size_per_block, u_int seg, u_int block_address);
        int Inode_getter_for_list(u_int inum, Inode &found_inode, std::list<Inode> ifile);
        int Inode_getter_for_array(u_int inum, Inode &found_inode, Inode[] ifile, int size);
        int Inode_get_last_inum_in_ifile();

        Direct_Block_Ptr* Inode_get_block_ptr();
} inode;

#endif
