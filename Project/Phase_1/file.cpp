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
int File_Create (Inode inode, std::string path, std::string filename, u_int inum, u_int filesize, int mode, int type)
{
    time_t cur_time;
    time(&cur_time);

    int status = inode.Inode_Initialization(filename, path, inum, cur_time, filesize, mode, type);
    if (status)
    {
        std::cerr << "File Create failed" << std::endl;
        return 1;
    }

    // Creates an empty file on a disk with a given file name
    std::ofstream ofs;
    ofs.open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::app);
    ofs.close();

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
    u_int block;
    u_int first_block = offset / BLOCKSIZE;
    int status = Log_Write(inum, block, BLOCK_SIZE, buffer, logAddress);

    if (status)
    {
        std::cerr << "Failed to write to inode" << std::endl;
        return 1;
    }
    // Open .ifile and retrieve all inodes in the file, then store them
    // temporarily into the temporary ifile for look up
    ifstream ifs;
    ifs.open(".ifile", std::ifstream::binary);

    if (ifs)
    {
        ifS.seekg(0, ifs.end);
        int len = ifs.tellg();
        ifs.seekg(0, ifs.beg);

        int size = len/sizeof(Inode);

        Inode temporary_ifile[size];
        ifs.read((char*)&inode, len);

        // Find and return the updated found_inode. If the target inode does not exist, print error and return 1.
        // Else, the returned value is the index of target inode in the temporary_ifile array
        Inode found_inode;
        int index = Inode.Inode_getter_for_array(inum, found_inode, temporary_ifile, size);
        if (index == 1)
        {
            std::cerr << "Inode does not exist" << std::endl;
            return 1;
        }
       
        // Below only handles direct 4 direct pointers 
        int current_segment = logAddress.segment;
        int current_block_addr = logAddress.block;;
        for (int i = 0; i < 4; i++)
        {
            status = Inode.Inode_Write(found_inode, i, current_segment, current_block_addr);
            if (status)
            {
                std::cerr << "Inode write failed" << std::endl;
                return 1;
            }
            current_block_addr = current_block_addr + BLOCKSIZE;
        }
        temporary_ifile[index] = found_inode;
        ifs.close();

        // Overwrite the entire ifile content with updated inode(s)
        // This, currently, is an inefficient way to update the .ifile, thus it may require improvement
        // of using the offset of a specific inode in the .ifile to only update that line.
        ofstream ostrm(".ifile", std::std::ios::binary | std::ios::out);
        ostrm.write((char*)&temporary_ifile, sizeof(Inode));
        ostrm.close();
    }
    else
    {
        std::cerr << "Failed to open .ifile" << std::endl;
        return 1;
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
    Inode found_inode;
    std::ifstream ifs;
    ifs.open (".ifile", std::ifstream::binary);

    if (ifs)
    {
        ifS.seekg(0, ifs.end);
        int len = ifs.tellg();
        ifs.seekg(0, ifs.beg);

        int size = len/sizeof(Inode);

        Inode temporary_ifile[size];
        ifs.read((char*)&inode, len);

        status = Inode.Inode_getter_for_array(inum, found_inode, temporary_ifile, size);
        if (status)
        {
            std::cerr << "Inode does not exist" << std::endl;
            return 1;
        }
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
    ifs.close();
    return 0;
}

void File_Free(u_int inum)
{

}
