/*
 *  Filename: file.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: The file layer is responsible for implementing the file abstraction. A file is represented by
 *  an inode containing the metadata for the file, including the file type (e.g. file or
 *  directory), size, and the flash addresses of the file's blocks.
 */

#include "file.h"

/*
 *********************************************************************
 * int
 * File_Create
 *
 * Parameters:
 *
 * Inode inode - an empty inode that requires initialization with the passed metadata
 * string path - path of a directory that the current file resides
 * string filename - name of a file
 * u_int inum - inode number that will be assigned to the inode
 * u_int filesize - size of the file
 * int mode - mode or the permission of the file
 * int type - file or directory
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * File_Create is an inode initialization function that calls Inode_Initialization function from Inode class
 *
 *********************************************************************
 */
int File_Create (Inode* inode, const char* path, const char* filename, u_int inum, u_int filesize, int mode, int type)
{
    time_t cur_time;
    time(&cur_time);

    // Initialize the inode
    int status = inode.Inode_Initialization(filename, path, inum, cur_time, filesize, mode, type);
    if (status)
    {
        std::cerr << "File Create failed" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * File_Write
 *
 * Parameters:
 *
 * u_int inum - inode number of a target inode that requires write
 * u_int offset - offset is the starting offset of the I/O in bytes
 * u_int length - the length of the I/O in bytes.
 * void* buffer -- buffer into which log is read
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * By calling Log_Write, File_Write finalizes the inode initialization.
 * With the filled logAddress information, File_Write initializes
 * inode's 4 direct pointers
 *
 *********************************************************************
 */
int File_Write(u_int inum, u_int offset, u_int length, void* buffer)
{
    LogAddress* logAddress;
    // Passing 0 fort the block number as it needs only the first block address
    int status = Log_Write(inum, 0, length, buffer, logAddress);

    if (status)
    {
        std::cerr << "Failed to write to inode" << std::endl;
        return 1;
    }

    Inode found_inode;
    int index = Inode_getter_for_list(inum, found_inode, ifile);
    if (index == 1)
    {
        std::cerr << "Unable to find the inode of given inum" << std::endl;
        return 1;
    }

    // Setting direct pointers of a file inode
    int current_segment = logAddress.segment;
    int current_block_addr = logAddress.block;;
    for (int i = 0; i < 4; i++)
    {
        status = found_inode.Inode_Write(i, current_segment, current_block_addr);
        if (status)
        {
            std::cerr << "Inode write failed" << std::endl;
            return 1;
        }
        current_block_addr = current_block_addr + num_bytes_in_block;
    }

    // Update the inode in ifile with the found_inode
    std::list<Inode>::iterator iter;
    int idx = 0;
    for (iter = ifile.begin(); iter != ifile.end(); iter++)
    {
        if (idx == index)
        {
            u_int inum_chck = found_inode.Inode_get_inum();
            if (inum == inum_chck)
                *iter = found_inode;
            else
                return 1;
        }
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * File_Read
 *
 * Parameters:
 *
 * u_int inum - inode number of a target inode that requires write
 * u_int offset - offset is the starting offset of the I/O in bytes
 * u_int length - the length of the I/O in bytes.
 * void* buffer -- buffer into which log is read
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * File_Read calls Log_read with passing the empty buffer to be filled.
 *
 *********************************************************************
 */
int File_Read(u_int inum, u_int offset, u_int length, void* buffer)
{
    // Find the target inode from the .ifile
    Inode found_inode;
    int index = Inode_getter(inum, found_inode);
    if (index == 1)
    {
        std::cerr << "Unable to find the inode of given inum" << std::endl;
        return 1;
    }

    LogAddress logAddress;
    Direct_Block_Ptr* block_ptr = found_inode.Inode_get_block_ptr();

    for (int i = 0; i < 4; i++)
    {
        logAddress.segment = *(block_ptr + i).segment;
        logAddress.block = *(block_ptr + i).block;

        status = Log_Read(logAddress, length, buffer);
        if (status)
        {
            std::cerr << "Log Read failed in file layer" << std::endl;
            return 1;
        }
    }


    // Update the last access time of the file
    found_inode.Inode_update_last_access_time();

    // Update the inode in ifile with the found_inode
    std::list<Inode>::iterator iter;
    int idx = 0;
    for (iter = ifile.begin(); iter != ifile.end(); iter++)
    {
        if (idx == index)
        {
            u_int inum_chck = found_inode.Inode_get_inum();
            if (inum == inum_chck)
                *iter = found_inode;
            else
                return 1;
        }
    }

    return 0;
}

void File_Free(u_int inum)
{

}
