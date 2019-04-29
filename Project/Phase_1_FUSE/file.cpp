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
int File::File_Create (Inode* inode, const char* path, const char* filename, u_int inum, int filesize, mode_t mode, mode_t type)
{
    // Get the current time
    time_t cur_time;
    time(&cur_time);

    // Initialize the inode
    int status = inode->Inode_Initialization(filename, path, inum, cur_time, filesize, mode, type);
    if (status)
    {
        std::cerr << "Error: Unable to create file" << std::endl;
        std::cerr << "File: file.cpp. Function: File_Create" << std::endl;
        return 1;
    }

    return 0;
}

int File::File_Open(const char* path, Inode* inode)
{
    char* ch_path = strdup(path);
    char* filename = basename(ch_path);

    int status = f_inode.Inode_Find_Inode(filename, path, inode);

    if (status > 0)
    {
        std::cerr << "Error: Unable to find the inode" << std::endl;
        std::cerr << "File: file.cpp. Function: File_Open" << std::endl;
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
int File::File_Write(u_int inum, off_t offset, size_t length, const char* buffer)
{
    LogAddress* logAddress;
    // Passing 0 for the block number as it needs only the first block address
    int status = Log_Write(inum, 0, length, buffer, logAddress);
    if (status)
    {
        std::cerr << "Failed to write to log" << std::endl;
        return 1;
    }

    if (inum != offset)
    {
        std::cerr << "Either offset or the inum is invalid" << std::endl;
        return 1;
    }

    Inode found_inode;
    status = f_inode.Inode_Getter(inum, offset, &found_inode);
    if (status)
    {
        std::cerr << "Failed to get the target inode" << std::endl;
        return 1;
    }

    // Setting direct pointers of a file inode
    int current_segment = logAddress->segment;
    int current_block_addr = logAddress->block;

    int number_of_blocks = ceil(length / (FLASH_SECTOR_SIZE * superBlock.block_size));

    for (int i = 0; i < number_of_blocks; i++)
    {
        // Update the currently opened file inode's block pointers
        status = found_inode.Inode_Write(i, current_segment, current_block_addr);
        if (status)
        {
            std::cerr << "Inode write failed" << std::endl;
            return 1;
        }
        // Update the current_block address and segs to the next block and segment addresses
        current_block_addr += 1;
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
 * u_int inum - inode number of a target inode that needs to be read
 * u_int offset - offset is the starting offset of the I/O in bytes
 * u_int length - the length of the I/O in bytes.
 * void* buffer - buffer into which log is read
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * File_Read calls Log_read with passing the empty buffer to be filled,
 * which the Log_Read will fill in the buffer based on the "length"
 * byte provided by the user mklfs -l <length> or the default of 32.
 *
 *********************************************************************
 */
int File::File_Read(u_int inum, off_t offset, size_t length, void* buffer)
{
    int status = 0;
    // Find the target inode from the .ifile
    Inode found_inode;
    status = found_inode.Inode_Getter(inum, offset, &found_inode);
    if (status)
    {
        std::cerr << "Failed to get the target inode" << std::endl;
        return 1;
    }

    LogAddress logAddress;

    Block_Ptr dir_block_ptr[4];
    std::list<Block_Ptr> ind_block_ptr;
    status = f_inode.Inode_Get_Block_Ptr(found_inode, dir_block_ptr, ind_block_ptr);

    if (status == 1)
    {
        std::cerr << "Failed to get either the direct or indirect pointers (or both) to the blocks" << std::endl;
        return 1;
    }

    int number_of_blocks = ceil(length / (FLASH_SECTOR_SIZE * superBlock.block_size));
    for (int i = 0; i < number_of_blocks; i++)
    {
        // Assign direct pointers to the log addresses
        if (i < 4)
        {
            logAddress.segment = dir_block_ptr[i].segment;
            logAddress.block = dir_block_ptr[i].block;
        }
        else
        {
            // Assign indirect pointers to the log address
            std::list<Block_Ptr>::iterator iter;
            for (iter = ind_block_ptr.begin(); iter != ind_block_ptr.end(); iter++)
            {
                Block_Ptr ind_ptr;
                logAddress.segment = iter->segment;
                logAddress.block = iter->block;
            }
        }
        status = Log_Read(logAddress, length, buffer);
        if (status)
        {
            std::cerr << "Log Read failed in file layer" << std::endl;
            return 1;
        }
    }

    // Update the last access time of the file
    status = found_inode.Inode_Update_Last_Access();

    return 0;
}

/*
 *********************************************************************
 * int
 * File_Free
 *
 * Parameters:
 *
 * u_int inum - inode number of the file
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * When the file gets deleted, we need to free up the occupied 
 * segments by the file inode
 *********************************************************************
 */
int File::File_Free(u_int inum)
{

}
