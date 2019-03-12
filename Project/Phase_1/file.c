/*
 *  Filename: file.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: The file layer is responsible for implementing the file abstraction. A file is represented by
 *  an inode containing the metadata for the file, including the file type (e.g. file or
 *  directory), size, and the flash addresses of the file's blocks.
 */

// Class header files list under here:
#include "flash.h"
#include "file.h"
#include "inode.h"

void File_Create (Inode inode, char* file, u_int inum, u_int filesize, int mode, int type)
{
    time_t cur_time;
    time(&cur_time);

    inode.Inode_Initialization(file, inum, cur_time, filesize, mode, type)
}

void File_Write(u_int inum, u_int offset, u_int length, u_int buffer)
{

}

void File_Read(u_int inum, u_int offset, u_int length, u_int buffer)
{

}

void File_Free(u_int inum)
{

}
