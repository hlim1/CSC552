/*
 *  Filename: directory.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/12/2019
 *  Author: Terrence Lim
 *  Summary:
 */

#include <assert.h>
#include "directory.h"

void Directory::Directory_initialization()
{
    Inode inode;
    int status = 0; // 0 is success and > 0 is fail
    DirMap dirMap[3]; // This holds the <name, inum> pair of file information of current directory

    u_int inum = 2; // The root directory's inode number is 2

    // 1. Create a root directory ("/": Forward slash)
    if (Directory_file_create ("/", "/", 0, S_IFDIR, S_IRWXU, inum) > 0)
    {
        std::cerr << "Root directory '/' creation failed." << std::endl;
        exit(1);
    }

    // 2. Create three special files ".", "..", and ".ifile"
    inum = 3;
    if (Directory_file_create ("/", ".", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.' creation failed." << std::endl;
        exit(1);
    }
    dirMap[0].name = ".";
    dirMap[0].inum = inum;
    inum = 4;
    if (Directory_file_create ("/", "..", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '..' creation failed." << std::endl;
        exit(1);
    }
    dirMap[1].name = "..";
    dirMap[1].inum = inum;
    inum = 5;
    if (Directory_file_create ("/", ".file", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.file' creation failed." << std::endl;
        exit(1);
    }
    dirMap[2].name = ".ifile";
    dirMap[2].inum = inum;

    // 3. Write the inode of root, "." and ".." to the .ifile
    ofs.open(".ifile", std::ios::binary | std::ios::out | std::ios::app);
    Inode fInode[2];
    inum = 3;
    for (int i = 0; i < 2; i++)
    {
        status = inode.Inode_getter_for_list(inum, fInode[i], ifile);
        if (status == 0)
        {
            ofs.write((char*)&fInode[i], sizeof(fInode[i]));
            inum++;
        }
        else
        {
            std::cerr << "Inode not found." << std::endl;
            exit(1);
        }
    }
    ofs.close();

    // 4. Add array of <name,inum> into root directory 
    ofs.open("/", std::ios::binary | std::ios::out | std::ios::app);
    for (int i = 0; i < 3; i++)
        ofs.write((char*)&dirMap, sizeof(dirMap));
    ofs.close();
}

int Directory::Directory_file_create (const char* path, std::string filename, u_int filesize, mode_t mode, mode_t type, u_int inum)
{
    Inode inode; // Inode for the new file
    File file;
    // Create a file. If successful, inode should be initialized with empty direct pointers
    file.File_Create(inode, path, filename, inum, filesize, mode, type);

    if (filename == "/")
        RootInode = inode;
    else
    {
        // Store generated inodes in memory ifile
        ifile.push_back(inode);
    }

    return 0;
}

void Directory_read(u_int inum, void* buffer, u_int offset)
{

}

/*
 *  Direct_file_Write()
 *  Given the inum of inode, it calls File_Write to initialize the inode's direct/indirect pointers.
 */
int Directory::Directory_file_write(u_int inum, void* buffer, u_int offset, u_int length)
{
    int status;
    File file;
    status = file.File_Write(inum, offset, length, buffer);

    if (status > 0)
    {
        std::cerr << "File write failed" << std::endl;
        exit(1);
    }
    
    return 0;
}
