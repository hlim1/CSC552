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
#include <fuse.h>

// Class header files list under here:
#include "directory.h"

const int NOTSUPPORTED = -1;
extern Directory* f_directory;

void* imp_init(struct fuse_conn_info *conn);
int imp_file_getattr(const char* path, struct stat*);

int imp_unlink(const char* path);
int imp_rmdir(const char* path);
int imp_access(const char* path, int mask);
int imp_file_open(const char* path, struct fuse_file_info* fi);
int imp_dir_open(const char* path, struct fuse_file_info* fi);
int imp_file_read(const char* path, char* buffer, size_t length, off_t offset, struct fuse_file_info* fi);
int imp_file_write(const char* path, const char* buffer, size_t length, off_t offset, struct fuse_file_info *fi);
int imp_statfs(const char* path, struct statvfs* stbuf);
int imp_file_release(const char* path, struct fuse_file_info *fi);
int imp_mkdir(const char* path, mode_t mode);
int imp_dir_release(const char* path, struct fuse_file_info* fi);
int imp_dir_read(const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
void imp_destroy(void *private_data);
int imp_file_create(const char* path, mode_t mode, struct fuse_file_info* fi);
int imp_link(const char* src_path, const char* dest_path);
int imp_symlink(const char* src_path, const char* dest_path);
int imp_truncate(const char* path, off_t offset);
int imp_rename(const char* org_path, const char* new_path);
int imp_chmod(const char * path, mode_t mode);
int imp_chown(const char * path, uid_t uid, gid_t id);

#endif
