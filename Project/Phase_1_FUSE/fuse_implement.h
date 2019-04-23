/*
 *  Filename: fuse_implement.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Author: Terrence Lim
 *
 *  Summary: This is a implementation of fuse functions.
 *  LFS.h will include this file in order to implement basic
 *  operations, such as open, read, write, and delete etc.
 *  This file is based on the libfuse and its fuse.h file.
 */

#ifndef FUSE_IMPLEMENT_H
#define FUSE_IMPLEMENT_H

// C++ std. libraries list here:
#include <iostream>

// C std. libraries list under here:
#include <stdio.h>
#include <sys/stat.h>

// Class header files list under here:
#include "fuse.h"

class Fuse_Implement {
    public:
        Fuse_Implement();
        int file_getattr(const char* path, struct stat*);
        int file_readline(const char* path, void* buffer, int length, off_t offset, struct fuse_file_info* fi);
        int unlink(const char* path);
        int rmdir(const char* path);
        int file_open(const char* path, struct fuse_file_info* fi);
        int file_read(const char* path, char* buffer, int length, off_t offset, struct fuse_file_info* fi);
        int file_write(const char* path, void* buffer, off_t offset, int length, struct fuse_file_info *);
        int statfs(const char* path, struct statvfs* stbuf);
        int file_release(const char* path, struct statvfs* stbuf);
        int mkdir(const char* path, mode_t mode);
        int dir_open(const char* path, struct fuse_file_info* fi);
        int dir_read(const char* path, void* buffer, off_t offset, struct fuse_file_info* fi);
        int dir_release(const char* path, struct fuse_file_info* fi);
        int init(struct fuse_file_info* fi);
        int destroy(void *private_data);
        int file_create(const char* path, , const char* filename, mode_t mode, mode_t type, struct fuse_file_info* fi);
        int link(const char* src_path, const char* dest_path);
        int symlink(const char* src_path, const char* dest_path);
        int truncate(const char* path, struct fuse_file_info* fi);
        int rename(const char* path, const char* filename, );
        int chmod(const char * path, mode_t mode, struct fuse_file_info *fi);
        int chown(const char * path, uid_t uid, gid_t id, struct fuse_file_info *fi);
};

#endif
