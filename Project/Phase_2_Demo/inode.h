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
#include <iostream>
#include <string.h>
#include <fstream>
#include <list>

// C std. libraries list under here:
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

// Class header files list under here:
#include "log.h"

struct Block_Ptr
{
    u_int segment;
    u_int block;
};

class Inode
{
    public:
        struct Container
        {
            char  m_file[20];    // File name
            char  m_path[50];    // Path of a file where it resides
            mode_t m_mode;    // Mode (or permission) of a file or directory
            mode_t m_type;  // Type of an Inode. Either a regular file (S_IFREG) or directory (S_IFDIR)
            uid_t  m_owner;   // Set the defualt to the current user id
            gid_t  m_group;   // Set the default to same as the owner's username

            u_int  m_filesize;
            u_int  m_inum;

            Block_Ptr m_direct_pointer[4]; // 4 direct pointers to the first 4 block of the of the file
            Block_Ptr* m_indirect_pointers; // Indirect pointer to a block of direct pointer Phase 2

            time_t m_last_modified;   // Holds the last modified time
            time_t m_last_accessed;   // Holds the last modified time
        } container;

        Inode() {};
        // A Constructor for inode with the passed metadata and allocate the inode in the ifile
        int Inode_Initialization(const char* filename, const char* path, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type);
        int Inode_Write(Inode* inode, size_t length, int number_of_blocks, LogAddress* logAddress);
        int Inode_Get_Inum(u_int &inum);
        int Inode_Get_Last_Inum(u_int &inum);
        int Inode_Find_Inode(const char* filename, const char* path, Inode* found_inode);
        int Inode_Update_Last_Access();
        int Inode_Getter(u_int inum, u_int offset, Inode* inode);
        int Inode_Get_Block_Ptr(Inode inode, Block_Ptr dir_block_ptr[4], Block_Ptr* ind_block_ptr);
        int Inode_Chmod(Inode* inode, mode_t mode);
        int Inode_Chown(Inode* inode, uid_t uid, gid_t id);
        int Inode_Rename(const char* path, const char* name);
        int Inode_Get_Inode(u_int inum, Inode* inode);

        int Inode_Check_Mode (uid_t uid);
        int Inode_Get_Total_Num_Of_Inodes();
        void Inode_Set_Stbuf(struct stat* stbuf, Inode file_inode);
};

#endif
