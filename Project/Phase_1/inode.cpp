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
int Inode::Inode_Initialization(std::string filename, std::string path, u_int filesize, u_int inum, time_t cur_time, mode_t mode, mode_t type)
{
    m_file     = filename;
    m_path     = path;
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

    return 0;
}

/*
 * Inode_getter_for_list()
 * With passed inum and the offset of a target inode, search through the ifile then
 * assign the target_inode pointer to the inode location, and return true, else return false
 */
int Inode::Inode_getter_for_list(u_int inum, Inode &found_inode, std::list<Inode> tempIfile)
{
    assert(!tempIfile.empty());
    Inode inode;
    for (std::list<Inode>::iterator iter = tempIfile.begin(); iter != tempIfile.end(); iter++)
    {
        if (iter->m_inum == inum)
        {
            found_inode = *iter;
            return 0;
        }
    }
    return 1;
}

/*
 * Inode_getter_for_array()
 * With passed inum and the offset of a target inode, search through the ifile then
 * assign the target_inode pointer to the inode location, and return true, else return false
 */
int Inode_getter_for_array(u_int inum, Inode &found_inode, Inode[] tempIfile, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (inum == tempIfile[i].m_inum)
        {
            found_inode = tempIfile[i];
            return i;
        }
    }
    return 1;
}

int Inode_get_last_inum_in_ifile()
{
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

        return temp_ifile[size-1].m_inum;
    }
    std::cerr << "Failed to open .ifile" << std::endl;
    return 1;
}

int Inode_Write(Inode &found_inode, u_int index, u_int seg, u_int block_address)
{
    found_inode.m_direct_pointer[index].segment = seg;
    found_inode.m_direct_pointer[index].block = block_address;;

    return 0;
}

Direct_Block_Ptr* Inode_get_block_ptr()
{
    return this->m_direct_pointer;
}
