/*
 *  Filename: file.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/1/2019
 *  Author: Terrence Lim
 *  Summary: The file layer is responsible for implementing the file abstraction. A file is represented by
 *  an inode containing the metadata for the file, including the file type (e.g. file or
 *  directory), size, and the flash addresses of the file's blocks.
 */

#ifndef FILE_H
#define FILE_H

class File
{
    public:
        void File_Create(Inode inode, u_int inum, int mode, int type);
        void File_Write(u_int inum, u_int offset, u_int length, u_int buffer);
        void File_Read(u_int inum, u_int offset, u_int length, u_int buffer);
        void File_Free(u_int inum);
}

#endif
