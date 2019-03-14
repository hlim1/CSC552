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
    DirMap dirMap[3];

    u_int inum = 2; // The root directory's inode number is 2

    // 1. Create a root directory ("/": Forward slash)
    if (directory_file_create ("/", "/", 0, S_IFDIR, S__IRWXU, inum) > 0)
    {
        std::cerr << "Root directory '/' creation failed." << std::endl;
        exit(1);
    }

    // 2. Create three special files ".", "..", and ".ifile"
    inum = 3
    if (directory_file_create ("/", ".", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.' creation failed." << std::endl;
        exit(1);
    }
    dirMap[0].name = ".";
    dirMap[0].inum = inum;
    inum = 4
    if (directory_file_create ("/", "..", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '..' creation failed." << std::endl;
        exit(1);
    }
    dirMap[1].name = "..";
    dirMap[1].inum = inum;
    inum = 5
    if (directory_file_create ("/", ".file", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.file' creation failed." << std::endl;
        exit(1);
    }
    dirMap[2].name = ".ifile";
    dirMap[2].inum = inum;

    ofs.open(".", std::ios::binary | std::ios::out | std::ios::app);
    ofs.close();
    ofs.open("..", std::ios::binary | std::ios::out | std::ios::app);
    ofs.close();
    ofs.open(".file", std::ios::binary | std::ios::out | std::ios::app);
    ofs.close();
    // 3. Add the inode of root, "." and ".." in the .ifile
    // 4. Add array of <name,inum> into root directory 
    ofs.open("/", std::ios::binary | std::ios::out | std::ios::app);
    for (int i = 0; i < 3; i++)
        ostrm.write((char*)&dirMap, sizeof(dirMap));
    ofs.close();
}

int directory_file_create (const char* path, std::string file, u_int filesize, mode_t mode, mode_t type, u_int inum)
{
    Inode inode;
    File dfile;
    dfile.File_Create(inode, file, inum, filesize, mode, type);    
}
