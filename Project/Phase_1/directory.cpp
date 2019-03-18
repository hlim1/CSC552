/*
 *  Filename: directory.cpp
 *  Course: CSs 552 Advanced Operating Systems
 *  Last modified: 3/12/2019
 *  Author: Terrence Lim
 *  Summary:
 */

#include <assert.h>
#include "directory.h"

/*
 *********************************************************************
 * int
 * Directory_initialization 
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * Initializes the directory layer. It creates the root directory "/"
 * and ".", "..", and ".ifile" files.
 *
 *********************************************************************
 */
int Directory::Directory_initialization()
{
    int status = 0; // 0 is success and > 0 is fail

    u_int inum = INUMOFROOTDIR;

    // Create a root directory ("/": Forward slash)
    if (Directory_create ("/", "/", S_IFDIR, S_IRWXU, inum) > 0)
    {
        std::cerr << "Root directory '/' creation failed." << std::endl;
        exit(1);
    }

    // Create .ifile when initializing the directory layer
    inum = INUMOFIFILE;
    if (Directory_file_create ("/", ".file", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.file' creation failed." << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_create
 *
 * Parameters:
 * std::string path - directory path
 * string dirname - current directory name that needs to be created
 * mode_t mode - mode or the permission of the file
 * mode_t type - file or directory
 * u_int inum - inode number of the directory
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * Creates a directory based on the passed parameters and create "."
 * and ".." files as every directory holds both as default.
 *
 *********************************************************************
 */
int Directory_create(std::string path, std::string dirname, mode_t mode, mode_t type, u_int inum)
{
    Inode inode;                // Inode for the new file
    File file;
    DirMap cur_directory;       // "."
    DirMap parent_directory;    // ".."
    u_int filesize = 0;

    // Create a file. If successful, inode should be initialized with empty direct pointers
    if (file.File_Create(inode, path, dirname, inum, filesize, mode, type) > 0)
    {
        std::cerr << "File create failed in Directory creation" << std::endl;
        return 1;
    }

    ifile.push_back(inode);

    // Every directory has '.' and '..' files
    inum = inum + 1;
    if (Directory_file_create ("/", ".", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.' creation failed." << std::endl;
        exit(1);
    }
    cur_directory.name = ".";
    cur_directory.inum = inum;
    directory.push_back(cur_directory);

    inum = inum + 1;
    if (Directory_file_create ("/", "..", 0, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '..' creation failed." << std::endl;
        exit(1);
    }
    parent_directory.name = "..";
    parent_directory.inum = inum;
    directory.push_back(parent_directory);

    return 0;
}

int Directory::Directory_file_create (std::string path, std::string filename, u_int filesize, int mode, int type, u_int inum)
{
    Inode inode; // Inode for the new file
    File file;

    // Create a file. If successful, inode should be initialized with empty direct pointers
    if (file.File_Create(inode, path, filename, inum, filesize, mode, type))
    {
        std::cerr << "File create failed in Directory file creation" << std::endl;
        return 1;
    }

    ifile.push_back(inode);

    return 0;
}

int Directory::Directory_file_read(u_int inum, void* buffer, u_int offset, u_int length)
{
    File file;
    int status = file.File_Read(inum, offset, length, buffer);

    if (status > 0)
    {
        std::cerr < "There was an error while reading a file in Directory read" << std::endl;
        return 1;
    }

    return 0;
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
