/*
 *  Filename: inode.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/13/2019
 *  Author: Terrence Lim
 *  Summary: 
 */

#include <assert.h>

#include "inode.h"

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
        container.m_direct_pointer[i] = NULL;

    container.m_mode        = mode;
    container.m_type        = type;
    container.m_last_modified = cur_time;
    container.m_last_accessed = cur_time;

    return 0;
}

/*
 * Updates the inode's direct and indirect pointers
 */
int Inode::Inode_Write(u_int index, u_int seg, u_int block_address)
{
    this->container.m_direct_pointer[index]->segment = seg;
    this->container.m_direct_pointer[index]->block = block_address;;

    return 0;
}

/*
 * Returns the inum of the current inode
 */
u_int Inode::Inode_get_inum()
{
    return this->container.m_inum;
}

/*
 * Returns the pointer to the direct pointer array
 */
Block_Ptr* Inode::Inode_get_block_ptr()
{
    return *this->container.m_direct_pointer;
}

/*
 * Updates the last access time to the file.
 * Always call when the file gets opened (read/write)
 */
void Inode::Inode_update_last_access()
{
    time_t cur_time;
    time(&cur_time);

    this->container.m_last_accessed = cur_time;
}
