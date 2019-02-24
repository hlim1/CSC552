/*
 *  Filename: file.h
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 2/24/2019
 *  Author: Terrence Lim
 *  Summary: 
 */

#ifndef FILE_H
#define FILE_H

class File
{
    public:
        void File_Create(u_int inum, char type);
        void File_Write(u_int inum, u_int offset, u_int length, u_int buffer);
        void File_Read(u_int inum, u_int offset, u_int length, u_int buffer);
        void File_Free(u_int inum);
}

#endif
