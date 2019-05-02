/*
 *  Filename: inode.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: 
 */

#include <assert.h>

#include "inode.h"
#include "log.h"

/*
 * When a new inode object gets created, it needs to be initialized with following metadata,
 * filename, permission, filesize, inode number, its type (directory or file), user info., group info.,
 * last modified time (which is the creation time at the initialization time), 4 direct pointers,
 * and 1 indirect pointer
 */
int Inode::Inode_Initialization(const char* filename, const char* path, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type)
{
    memcpy(container.m_file, filename, strlen(filename)+1);
    memcpy(container.m_path, path, strlen(path)+1);
    container.m_filesize = filesize;
    container.m_inum     = inum;
    container.m_owner    = getuid();
    container.m_group    = getgid();

    // Inode gets created before the actual file, thus initialize the direct pointers to null at initialization stage.
    // Then, set them to the actual blocks when writing to the files
    for (int i = 0; i < 4; i++)
    {
        container.m_direct_pointer[i].segment = 0;
        container.m_direct_pointer[i].block = 0;
    }
    container.m_indirect_pointers = NULL;

    container.m_mode          = mode;
    container.m_type          = type;
    container.m_last_modified = cur_time;
    container.m_last_accessed = cur_time;

    return 0;
}

/*
 * Updates the inode's direct and indirect pointers
 */
int Inode::Inode_Write(Inode* inode, size_t length, int number_of_blocks, LogAddress* logAddress)
{
    int current_segment = logAddress->segment;
    int current_block_addr = logAddress->block;
    std::list<Block_Ptr> ptrs;

    for (int  i = 0; i < number_of_blocks; i++)
    {
        // Updating first 4 direct pointers
        if (i < 4)
        {
            inode->container.m_direct_pointer[i].segment = current_segment;
            inode->container.m_direct_pointer[i].block = current_block_addr++;
        }
        else
        {
            // Temporary block that will be pushed back to the indirect pointer list
            Block_Ptr dir_ptr;
            dir_ptr.segment = current_segment;
            dir_ptr.block = current_block_addr;

            ptrs.push_back(dir_ptr);
        }
    }

    // Set an indirect pointers to the list of direct pointers to the blocks
    // inode->container.m_indirect_pointers = &(ptrs);

    return 0;
}

/*
 * Returns the inum of the current inode
 * If the inum is less than the size of a single Inode container,
 * it's an error that should return return value of 1
 */
int Inode::Inode_Get_Inum(u_int& inum)
{
    inum = this->container.m_inum;
    if (inum != 0 && inum < sizeof(Inode::Container))
        return 1;
    return 0;
}

/*
 * Updates the last access time to the file.
 * Always call when the file gets opened (read/write)
 */
int Inode::Inode_Update_Last_Access()
{
    time_t cur_time;
    time(&cur_time);

    this->container.m_last_accessed = cur_time;
    return 0;
}

/*
 * Find and returns the inode of the target file with its metadata (name and path).
 * This is useful when we do not know or have any information about the inode, such as
 * inum, other than the file name and path.
 */
int Inode::Inode_Find_Inode(const char* filename, const char* path, Inode* found_inode)
{
    int status = 1;
    // Open .ifile
    std::ifstream ifile(".ifile", std::ifstream::binary);
    if (ifile)
    {
        ifile.seekg(0, ifile.end);
        int length = ifile.tellg();
        int size = length / sizeof(Inode::Container);
        ifile.seekg(0, ifile.beg);

        Inode inodes[size];
        ifile.read((char*)&inodes, length);

        for (int i = 0; i < size; i++)
        {
            if (strcmp(inodes[i].container.m_path, path) == 0 && strcmp(inodes[i].container.m_file, filename) == 0)
            {
                //memcpy(found_inode, inodes[i], sizeof(Inode::Container));
                *found_inode = inodes[i];
                status = 0;
            }   
        }
        if (status == 1)
        {
            std::cerr << "Unable to find the correct inode for either the filename or path or both" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "File: inode.cpp. Function: Inode_Find_Inode" << std::endl;
        return 1;
    }
    return 0;
}

int Inode::Inode_Get_Inode(u_int inum, Inode* inode)
{
     int status = 1;
    // Open .ifile
    std::ifstream ifile(".ifile", std::ifstream::binary);
    if (ifile)
    {
        ifile.seekg(0, ifile.end);
        int length = ifile.tellg();
        int size = length / sizeof(Inode::Container);
        ifile.seekg(0, ifile.beg);

        Inode inodes[size];
        ifile.read((char*)&inodes, length);

        for (int i = 0; i < size; i++)
        {
            if (inodes[i].container.m_inum == inum)
            {
                //memcpy(found_inode, inodes[i], sizeof(Inode::Container));
                *inode = inodes[i];
                status = 0;
            }   
        }
        if (status == 1)
        {
            std::cerr << "Unable to find the correct inode for either the filename or path or both" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "File: inode.cpp. Function: Inode_Find_Inode" << std::endl;
        return 1;
    }
    return 0;
}

/*
 *  When we know about the inum and the offset of the file, use the offset to retrieve the inode and return the inode.
 */
int Inode::Inode_Getter(u_int inum, u_int offset, Inode* inode)
{
     // Open .ifile
    std::ifstream ifile(".ifile", std::ifstream::binary);
    if (ifile)
    {
        ifile.seekg(offset);
        ifile.read((char*)&inode, sizeof(Inode::Container));
        if (inode->container.m_inum != inum)
        {
            std::cerr << "Unable to find the correct inode for either the filename or path or both" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "Failed to open .ifile in Inode_find_inum function" << std::endl;
        return 1;
    }
    return 0;   
}

/*
 *  Updates the passed empty direct pointer array and indirect pointer list with the current
 *  inode's pointers to blocks
 */
int Inode::Inode_Get_Block_Ptr(Inode inode, Block_Ptr dir_block_ptr[4], Block_Ptr* ind_block_ptr)
{
    int status = 0;
    // If the first m_direct_pointer is NULL (empty), it's an error that the blocks are not properly
    // initialized with the correct value
    if (inode.container.m_direct_pointer[0].segment == 0 || inode.container.m_direct_pointer[0].block == 0)
    {
        return 1;
    }

    // Set direct pointers
    for (int i = 0; i < 4; i++)
    {
        dir_block_ptr[i].segment = inode.container.m_direct_pointer[i].segment;
        dir_block_ptr[i].block = inode.container.m_direct_pointer[i].block;
        status = 0;
    }

    if (status == 1)
    {
        std::cerr << "Direct pointer array is empty" << std::endl;
        return 1;
    }

    ind_block_ptr = inode.container.m_indirect_pointers;

    return 1;
}

int Inode::Inode_Get_Last_Inum(u_int &inum)
{
    // Open .ifile
    std::ifstream ifile(".ifile", std::ifstream::binary);
    if (ifile)
    {
        ifile.seekg(0, ifile.end);
        int length = ifile.tellg();
        int size = length / sizeof(Inode::Container);
        ifile.seekg(0, ifile.beg);

        Inode inodes[size];
        ifile.read((char*)&inodes, length);

        inum = inodes[0].container.m_inum;
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;   
}

/*
 *  Updates the mode of a file or directory
 */
int Inode::Inode_Chmod(Inode* inode, mode_t mode)
{
    int status = inode->Inode_Check_Mode(mode);
    if (status > 0)
    {
        std::cerr << "Invalid mode was received in Inode_chmod. Input mode: " << mode << std::endl;
    }

    inode->container.m_mode = mode;
    return 0;
}

/*
 *  Update the uid and/or gid of a file or directory
 */
int Inode::Inode_Chown(Inode* inode, uid_t uid, gid_t id)
{
    inode->container.m_owner = uid;
    inode->container.m_group = id;

    return 0;
}

/*
 *  Check the input mode of a file
 */
int Inode::Inode_Check_Mode(uid_t uid)
{
    int status = 1;

    uid_t fileMode[15] = {S_IRWXU, S_IRUSR, S_IWUSR, S_IXUSR, S_IRWXG, S_IRGRP, S_IWGRP, S_IXGRP,
                        S_IRWXO, S_IROTH, S_IWOTH, S_IXOTH, S_ISUID, S_ISGID, S_ISVTX};

    for (int i = 0; i < 15; i++)
    {
       if (fileMode[i] == uid)
          status = 0; 
    }

    return status;
}

/*
 *  Update the name and path of a file or directory
 */
int Inode::Inode_Rename(const char* new_path, const char* new_name)
{
    memcpy(container.m_file, new_name, strlen(new_name)+1);
    memcpy(container.m_path, new_path, strlen(new_path)+1);

    return 0;
}

void Inode::Inode_Set_Stbuf(struct stat* stbuf, Inode file_inode)
{
    stbuf->st_size = file_inode.container.m_filesize;
    stbuf->st_ino = file_inode.container.m_inum;
    stbuf->st_uid = file_inode.container.m_owner;
    stbuf->st_gid = file_inode.container.m_group;
    stbuf->st_mode = file_inode.container.m_mode | file_inode.container.m_type;
    stbuf->st_blksize = superBlock.block_size;
}
