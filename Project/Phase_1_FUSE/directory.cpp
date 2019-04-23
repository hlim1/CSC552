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
 * Initializes the directory layer. It creates a root directory "/"
 * that is actually a special file that will hold map of <filename, inum>.
 * Then, ".", "..", and ".ifile" files gets generated as a defualt that
 * each will have their own unique inum and stored in the directory file
 * except the .ifile, which will be store in the checkpoint to break
 * the circularity issue that can be arised by an .ifile storing its
 * own inum.
 *********************************************************************
 */
int Directory::Directory_initialization()
{
    int status = 0; // 0 is success and > 0 is fail

    u_int inum = INUMOFROOTDIR; // Always zero(0)
    if (inum != 0)
    {
        std::cerr << "Invalid inum for the root directory" << std::endl;
        return 1;
    }

    // Creates a root directory ("/": Forward slash)
    if (Directory_create ("/", "/", S_IFDIR, S_IRWXU, inum) > 0)
    {
        std::cerr << "Root directory '/' creation failed." << std::endl;
        return 1;
    }

    // Creates .ifile when initializing the directory layer
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
 * const char* path - directory path
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
int Directory::Directory_create(const char* path, const char* dirname, mode_t mode, mode_t type, u_int inum)
{
    Inode inode;                // Inode for the new file
    DirMap cur_directory;       // "."
    DirMap parent_directory;    // ".."
    u_int filesize = 0;

    // Creates a new directory, which really is simply a file that will hold <name, inum> list,
    // with given directory name and metadata.
    if (Directory_file_create (path, dirname, filesize, mode, type, inum) > 0)
    {
        std::cerr << "file '.' creation failed." << std::endl;
        return 1;
    }

    // Every directory has '.' and '..' files
    char current[] = ".";
    inum = inum + 1;
    if (Directory_file_create ("/", current, filesize, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '.' creation failed." << std::endl;
        return 1;
    }
    memcpy(cur_directory.name, current, strlen(current)+1);
    cur_directory.inum = inum;
    directory.push_back(cur_directory);

    char parent[] = "..";
    inum = inum + 1;
    if (Directory_file_create ("/", parent, filesize, S_IFREG, S_IRUSR, inum) > 0)
    {
        std::cerr << "file '..' creation failed." << std::endl;
        return 1;
    }
    memcpy(cur_directory.name, parent, strlen(parent)+1);
    parent_directory.inum = inum;
    directory.push_back(parent_directory);

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_Read
 *
 * Parameters:
 * const char* path - directory path
 * int length - Length of bytes to read
 * void* buffer - The directory entries will be passed and stored in
 *                this buffer
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * The purpose of this function is to insert the directory entries,
 * which are the contents of directory file of <name, inum> pairs,
 * into the passed buffer.
 *********************************************************************
 */
int Directory::Directory_read(const char* path, int length, void* buffer)
{
    int status = 0;
    Inode dirInode;
    File file;

    char* dirname = basename(path);

    // Find the inode of the target directory with its path and name
    status = dirInode.Inode_Find_Inode(dirname, path, &dirInode);
    if (status > 0)
    {
        std::cerr << "Error while finding the inode" << std::endl;
        return 1;
    }
    
    u_int inum;
    status = dirInode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error while retrieving the inum" << std::endl;
        return 1;
    }

    int offset;
    status = file.File_Read(inum, offset, length, buffer);
    if (status > 0)
    {
        std::cerr << "Error while reading the file" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_write
 *
 * Parameters:
 * const char* path - a path to the directory
 * void* buffer - a buffer that needs to write into
 * u_int offset - an offset of an inode in the ifile
 * u_int length - a length of bytes to write into the buffer
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * When a file or directory gets opened and modified in the
 * directory, this function will call the file_write to update the
 * inode of the current directory.
 *********************************************************************
 */
int Directory::Directory_write(const char* path, void* buffer, u_int offset, u_int length)
{
    char* dirname = basename(path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(dirname, path, inode);
    if (status > 0)
    {
         std::cerr << "Error while retrieving an inode in the Directory_write" << std::endl;
         return 1;
    }

    int inum;
    status = inode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error while retrieving the inum of an inode in the Directory_write" << std::endl;
        return 1;
    }

    File file;
    status = file.File_Read(inum, offset, length, buffer);
    if (status > 0)
    {
        std::cerr << "Error while reading a file in the Directory_write" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_free
 *
 * Parameters:
 * u_int inum - inode number of the directory
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * When the directory gets deleted, we need to free up the occupied
 * segement
 *********************************************************************
 */
int Directory::Directory_free(const char* path)
{

}

/*
 *********************************************************************
 * int
 * Directory_file_create
 *
 * Parameters:
 * const char* path - directory path
 * string filename - current file name that needs to be created
 * u_int filesize - size of a file
 * mode_t mode - mode or the permission of the file
 * mode_t type - file or directory
 * u_int inum - inode number of the directory
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * This function will call File_Create function, which is a public
 * member function of a File class, to create a new file with metadata
 * passed to its arguments
 *********************************************************************
 */
int Directory::Directory_file_create (const char* path, const char* filename, u_int filesize, int mode, int type, u_int inum)
{
    Inode new_inode; // Inode for the new file
    File new_file;

    // Create a file. If successful, inode should be initialized with empty direct pointers
    if (new_file.File_Create(&new_inode, path, filename, inum, filesize, mode, type))
    {
        std::cerr << "File create failed in Directory file creation" << std::endl;
        return 1;
    }

    // If the currently handling file is ifile, then store it into the globally accessible
    // inode_of_ifile, so it can be accessed in the log layer.
    // Else, store it to the globally accessible inode_of_current_file object for the log layer, again.
    if (filename == ".ifile")
        inode_of_ifile = new_inode;
    else
        inode_of_current_file = new_inode;

    return 0;
}

/*
 *  int
 *  Direct_file_Write
 *  Given the inum of inode, it calls File_Write to initialize the inode's direct/indirect pointers.
 */
int Directory::Directory_file_write(const char* path, void* buffer, u_int offset, u_int length)
{
    char* dirname = basename(path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(dirname, path, inode);
    if (status > 0)
    {
         std::cerr << "Error while retrieving an inode in the Directory_write" << std::endl;
         return 1;
    }

    int inum;
    status = inode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error while retrieving the inum of an inode in the Directory_write" << std::endl;
        return 1;
    }

    if (inum < 2)
    {
        std::cerr << "Invalid inum passed to Directory file write" << std::endl;
        return 1;
    }

    File file;
    status = file.File_Write(inum, offset, length, buffer);

    if (status > 0)
    {
        std::cerr << "File write failed" << std::endl;
        return 1;
    }
    
    return 0;
}

int Directory::Directory_file_read(const char* path, void* buffer, u_int offset, u_int length)
{
    char* dirname = basename(path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(dirname, path, inode);
    if (status > 0)
    {
         std::cerr << "Error while retrieving an inode in the Directory_write" << std::endl;
         return 1;
    }

    int inum;
    status = inode.Inode_Get_Inum(&inum);
    if (status > 0)
    {
        std::cerr << "Error while retrieving the inum of an inode in the Directory_write" << std::endl;
        return 1;
    }

    if (inum < 2)
    {
        std::cerr << "Invalid inum passed to Directory file write" << std::endl;
        return 1;
    }
    
    if (inum < sizeof(Inode::Container))
    {
        std::cerr << "Invalid inum passed to Directory_file_read " << inum << std::endl;
        return 1;
    }

    File file;
    status = file.File_Read(inum, offset, length, buffer);
    if (status > 0)
    {
        std::cerr << "Error while reading a file in Directory_file_read" << std::endl;
        return 1;
    }

    return 0;
}

int Directory::Directory_file_free(const char* path)
{

}
