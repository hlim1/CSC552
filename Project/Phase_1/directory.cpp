/*
 *  Filename: directory.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/12/2019
 *  Author: Terrence Lim
 *  Summary:
 */

#include <directory.h>

void Directory_initialization ()
{
    int status = 0; // 0 is success and > 0 is fail

    // 1. Create an inode for a root directory
    Inode* dInode;
    u_int inum = 2; // The root directory's inode number is 2
    // 2. Create a root directory ("/": Forward slash)
    File rfile;
    if (rfile.File_Create (dInode, "/", inum, S_IFDIR, S__IRWXU) > 0)
    {
        std::cerr << "Root directory '/' creation failed." << std::endl;
        exit(1);
    }
    // 3. Create three special files ".", "..", and ".ifile"
    // 4. Add the inode of root, "." and ".." in the .ifile
    // 5. Add the inode of .ifile in the checkpoint
}

void directory_file_create (const char* path, mode_t mode, uid_t owner, gid_t group)
{

}
