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
 * set_inode()
 * When a new inode object gets created, it needs to be initialized with following metadata,
 * filename, permission, filesize, inode number, its type (directory or file), user info., group info.,
 * last modified time (which is the creation time at the initialization time), 4 direct pointers,
 * and 1 indirect pointer
 */
void Inode::Inode_Initialization(std::string file, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type)
{
    m_file     = file;
    m_filesize = filesize;
    m_inum     = inum;
    m_owner    = getuid();
    m_group    = getgid();

    // Inode gets created before the actual file, thus initialize the direct pointers to null at initialization stage.
    // Then, set them to the actual blocks when writing to the files
    for (int i = 0; i < 4; i++)
        m_direct_pointer[i] = NULL;

    m_mode        = mode;
    m_type        = type;
    last_modified = cur_time; 
}

/*
 * Inode_getter()
 * With passed inum and the offset of a target inode, search through the ifile then
 * assign the target_inode pointer to the inode location, and return true, else return false
 */
int Inode::Inode_getter(u_int inum, Inode found_inode, std::list<Inode> ifile)
{
    assert(!ifile.empty());
    Inode inode;
    for (std::list<Inode>::iterator iter = ifile.begin(); iter != ifile.end(); iter++)
    {
        if (iter->m_inum == inum)
        {
            found_inode = *iter;
            return 0;
        }
    }
    return 1;
}
