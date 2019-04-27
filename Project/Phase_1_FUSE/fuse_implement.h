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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

// Class header files list under here:
#include "fuse.h"
#include "directory.h"

const int NOTSUPPORTED = -1;

class Fuse_Implement {

    static Directory* directory;

    public:
        Fuse_Implement();
        static int imp_init(struct fuse_conn_info *conn);
        static int imp_file_getattr(const char* path, struct stat*);
        static int imp_unlink(const char* path);
        static int imp_rmdir(const char* path);
        static int imp_file_open(const char* path, struct fuse_file_info* fi);
        static int imp_file_read(const char* path, char* buffer, int length, off_t offset, struct fuse_file_info* fi);
        static int imp_file_write(const char* path, void* buffer, off_t offset, int length, struct fuse_file_info *fi);
        static int imp_statfs(const char* path, struct statvfs* stbuf);
        static int imp_file_release(const char* path, struct fuse_file_info *fi);
        static int imp_mkdir(const char* path, mode_t mode);
        static int imp_dir_open(const char* path, struct fuse_file_info* fi);
        static int imp_dir_read(const char* path, fuse_fill_dir_t filler, void* buffer, off_t offset, struct fuse_file_info* fi);
        static int imp_dir_release(const char* path, struct fuse_file_info* fi);
        static int imp_destroy(void *private_data);
        static int imp_file_create(const char* path, mode_t mode, struct fuse_file_info* fi);
        static int imp_link(const char* src_path, const char* dest_path);
        static int imp_symlink(const char* src_path, const char* dest_path);
        static int imp_truncate(const char* path, int size);
        static int imp_rename(const char* org_path, const char* new_path, unsigned int flags);
        static int imp_chmod(const char * path, mode_t mode, struct fuse_file_info *fi);
        static int imp_chown(const char * path, uid_t uid, gid_t id, struct fuse_file_info *fi);
};

/*** Edit by Sabin ***/
//extern Fuse_Implement fuse_implement;
/*** End edit ***/

static struct fuse_operations fuse_imp_oper = {
    .init       =   Fuse_Implement::imp_init,
    .getattr    =   Fuse_Implement::imp_file_getattr,
    .unlink     =   Fuse_Implement::imp_unlink,
    .rmdir      =   Fuse_Implement::imp_rmdir,
    .open       =   Fuse_Implement::imp_file_open,
    .opendir    =   Fuse_Implement::imp_dir_open,
    .read       =   Fuse_Implement::imp_file_read,
    .write      =   Fuse_Implement::imp_file_write,
    .statfs     =   Fuse_Implement::imp_statfs,
    .release    =   Fuse_Implement::imp_file_release,
    .releasedir =   Fuse_Implement::imp_dir_release,
    .destroy    =   Fuse_Implement::imp_destroy,
    .create     =   Fuse_Implement::imp_file_create,
    .link       =   Fuse_Implement::imp_link,
    .symlink    =   Fuse_Implement::imp_symlink,
    .truncate   =   Fuse_Implement::imp_truncate,
    .rename     =   Fuse_Implement::imp_rename,
    .chmod      =   Fuse_Implement::imp_chmod,
    .chown      =   Fuse_Implement::imp_chown,
};

#endif
