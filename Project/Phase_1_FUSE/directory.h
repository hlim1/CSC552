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

//#include <file.h>
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
        Directory() {};
        // Functions for directory
        int Directory_initialization(struct fuse_conn_info* conn);
        int Directory_create(const char* path, mode_t mode);
        int Directory_open(const char* path, struct fuse_file_info* fi);
        int Directory_read(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
        int Directory_write(const char* path, void* buffer, off_t offset, size_t length);
        int Directory_free(const char* path);
        int Directory_chmod(const char* path, mode_t mode);
        int Directory_chown(const char* path, uid_t uid, gid_t id);

        // Functions for file
        int Directory_file_create(const char* path, mode_t mode, struct fuse_file_info* fi);
        int Directory_file_open(const char* path, struct fuse_file_info* fi);
        int Directory_file_write(const char* path, void* buffer, off_t offset, size_t length);
        int Directory_file_read(const char* path, char* buffer, off_t offset, size_t length);
        int Directory_file_truncate(const char* path, int size);
        int Directory_file_rename(const char* org_path, const char* new_path);
        int Directory_file_free(const char* path);
};

/*** Edit by Sabin ***/
extern Directory dir;
/*** End Edit ***/ 

#endif
