#include "directory.h"

/*** Edit by Sabin ***/
 Directory dir;
/*** End Edit ***/ 

Inode inode_of_ifile;
std::list<Inode> list_of_inodes;
std::list<DirMap> directories;

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
void* Directory::Directory_initialization(struct fuse_conn_info* conn)
{
    u_int inum = INUMOFROOTDIR; // Always zero(0)
    if (inum != 0)
    {
        std::cerr << "Error: Invalid inum for the root directory" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_initialization" << std::endl;
    }

    // Creates a root directory ("/": Forward slash)
    // Create root directory with mode(permission) with read and write for owner
    if (Directory_create ("/", S_IRUSR | S_IWUSR) > 0)
    {
        std::cerr << "Error: Unable to create Root directory '/'" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_initialization" << std::endl;
    }

    // Creates .ifile when initializing the directory layer
    inum = INUMOFIFILE;
    // Create .file with a regular file mode, and read and writer permossions for owner
    if (Directory_file_create (".ifile", S_IFREG, S_IRUSR | S_IWUSR, NULL) > 0)
    {
        std::cerr << "Error: file Unable to create '.file'" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_initialization" << std::endl;
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
 * mode_t mode - mode or the permission of the file
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
int Directory::Directory_create(const char* path, mode_t mode)
{
    char* ch_path = strdup(path);
    char* dirname = basename(ch_path);

    Inode inode;                // Inode for the new file
    DirMap cur_directory;       // "."
    DirMap parent_directory;    // ".."

    // Set type to directory
    mode_t type = S_IFDIR;

    // Retrieve the last inode number in the .ifile
    u_int inum;
    int status = inode.Inode_Get_Last_Inum(inum);
    if (status > 0)
    {
        // This is a case where both the .file and in-memory list are empty.
        if (list_of_inodes.empty())
        {
            // Only case that it is valid is when it's a root directory creation
            if (path != "/")
            {
                std::cerr << "Error: Unable to retrieve the last inum of inode - from file" << std::endl;
                std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
                return 1;
            }
            else
            {
                inum = 0;
            }
        }
        else
        {
            status = 0;
            Inode last_inode_in_mem = list_of_inodes.back();
            status = last_inode_in_mem.Inode_Get_Inum(inum);
            if (status > 0)
            {
                std::cerr << "Error: Unable to retrieve the last inum of inode - from list" << std::endl;
                std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
                return 1;
            }
        }
    }

    // Creates a new directory, which really is simply a file that will hold <name, inum> list
    if (Directory_file_create (path, type, mode, NULL) > 0)
    {
        std::cerr << "Error: Unable to create Directory file" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
        return 1;
    }

    // Every directory has '.' and '..' files
    inum = inum + 1;
    if (Directory_file_create ("/.", S_IFREG, S_IRUSR, NULL) > 0)
    {
        std::cerr << "Error: Unable to create file '.'" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
        return 1;
    }
    // Store file name "." and inum into the directory object
    memcpy(cur_directory.name, ".", strlen(".")+1);
    cur_directory.inum = inum;
    directories.push_back(cur_directory);

    inum = inum + 1;
    if (Directory_file_create ("/..", S_IFREG, S_IRUSR, NULL) > 0)
    {
        std::cerr << "Error: Unable to create file '..'" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
        return 1;
    }
    // Store file name ".." and inum into the directory object
    memcpy(cur_directory.name, "..", strlen("..")+1);
    parent_directory.inum = inum;
    // Store the object in the on memory list for later saving to the disk at checkpoint
    directories.push_back(parent_directory);

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_read
 *
 * Parameters:
 *  const char* path - directory path
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
// int Directory::Directory_read( const char* path, void* buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
int Directory::Directory_read(const char* path, fuse_fill_dir_t filler, void* buffer, off_t offset, struct fuse_file_info* fi)

{
    int status = 0;
    Inode dirInode;
    File file;

    char* ch_path = strdup(path);
    char* dirname = basename(ch_path);

    // Find the inode of the target directory with its path and name
    status = dirInode.Inode_Find_Inode(dirname, path, &dirInode);
    if (status > 0)
    {
        if (list_of_inodes.empty())
        {
            std::cerr << "Error: .ifile is empty" << std::endl;
            std::cerr << "File: directory.cpp. Function: Directory_read" << std::endl;
            return 1;
        }
        std::list<Inode>::iterator iter;
        for (iter = list_of_inodes.begin(); iter != list_of_inodes.end(); iter++)
        {
            if(strcmp(iter->container.m_path, path) == 0 && strcmp(iter->container.m_file, dirname))
            {
                if (iter->container.m_type == S_IFDIR)
                {
                    dirInode = *iter;
                    status = 0;
                }
            }
        }
    }
    
    u_int inum;
    status = dirInode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error while retrieving the inum" << std::endl;
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
 *  const char* path - a path to the directory
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

/*** Edit sabin ***/
// int Directory::Directory_write( const char* path, void* buffer, off_t offset, size_t length)
int Directory::Directory_write(const char* path, void* buffer, off_t offset, size_t length)
/*** End edit ***/

{
    char* ch_path = strdup(path);
    char* dirname = basename(ch_path);

    Inode inode;
    
    /*** Edit sabin ***/
    int status = inode.Inode_Find_Inode(dirname, path, &inode);
    /*** end edit ***/

    if (status > 0)
    {
         std::cerr << "Error while retrieving an inode in the Directory_write" << std::endl;
         return 1;
    }

    u_int inum;
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
    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_create
 *
 * Parameters:
 * const char* path - directory path
 * mode_t type - file or directory
 * mode_t mode - mode or the permission of the file
 * struct fuse_file_info* fi - fuse info.
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * This function will call File_Create function, which is a public
 * member function of a File class, to create a new file with metadata
 * passed to its arguments
 *********************************************************************
 */
int Directory::Directory_file_create (const char* path, mode_t type, mode_t mode, struct fuse_file_info* fi)
{
    // Since a direct conversion from const char* to char* is invalid,
    // the program is converting the const char* to char* using char* strdup(const char*) function.
    char* ch_path = strdup(path);
    // Then, extracts the basename, which is either the directory or file name, from the path
    char* filename = basename(ch_path);
    // A new inode object for the new file
    Inode new_inode;
    // A new file object for the new file
    File new_file;
    // Since a new file has no content held, the file size is initialize to 0
    int filesize = 0;
    // Retrieve the last inum in the .ifile
    u_int inum;
    int status = new_inode.Inode_Get_Last_Inum(inum);
    if (status > 0)
    {
        // This is a case where both the .file and in-memory list are empty.
        if (list_of_inodes.empty())
        {
            // Only case that it is valid is when it's a root directory creation
            if (path != "/")
            {
                std::cerr << path << std::endl;
                std::cerr << "Error: Unable to retrieve the last inum of inode - from file" << std::endl;
                std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
                return 1;
            }
            else
            {
                inum = 0;
            }
        }
        else
        {
            status = 0;
            Inode last_inode_in_mem = list_of_inodes.back();
            status = last_inode_in_mem.Inode_Get_Inum(inum);
            inum += sizeof(Inode::Container);
            if (status > 0)
            {
                std::cerr << "Error: Unable to retrieve the last inum of inode - from list" << std::endl;
                std::cerr << "File: directory.cpp. Function: Directory_create" << std::endl;
                return 1;
            }
        }
    }
    else
    {
        inum += sizeof(Inode::Container);
    }
    // Create a file. If successful, inode should be initialized with empty direct pointers
    status = new_file.File_Create(&new_inode, path, filename, inum, filesize, mode, type);
    if (status > 0)
    {
        std::cerr << "Error: Unable to create a new file" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_create" << std::endl;
        return 1;
    }

    // If the currently handling file is ifile, then store it into the globally accessible
    // inode_of_ifile, so it can be accessed in the log layer.
    // Else, store it to the globally accessible inode_of_current_file object for the log layer, again.
    if(strcmp(filename, ".ifile") == 0)
        inode_of_ifile = new_inode;
    else
    {
        list_of_inodes.push_back(new_inode);
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_open
 *
 * Parameters:
 * const char* path - directory path
 * struct fuse_file_info* fi - fuse info.
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * This function will call File_Create function, which is a public
 * member function of a File class, to create a new file with metadata
 * passed to its arguments
 *********************************************************************
 */
int Directory::Directory_file_open(const char* path, struct fuse_file_info* fi)
{
    Inode inode;
    File file;
    int status = file.File_Open (path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to open a file" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_open" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_write
 *
 * Parameters:
 * const char* path - file path
 * void* buffer - a buffer that holds contents to be written to file
 * off_t offset - offset of .ifile
 * size_t length - length in bytes to be written to the log
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * Given the inum of inode, it calls File_Write to initialize the inode's direct/indirect pointers.
 *********************************************************************
 */
int Directory::Directory_file_write(const char* path, const char* buffer, off_t offset, size_t length)
{
    char* ch_path = strdup(path);
    char* dirname = basename(ch_path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(dirname, path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to find the inode" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_write" << std::endl;
        return 1;
    }

    // Retrieve the inum of the current inode
    u_int inum;
    status = inode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inum" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_write" << std::endl;
        return 1;
    }

    if (inum < 2)
    {
        std::cerr << "Error: Invalid inum. Retrieved inum < 2." << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_write" << std::endl;
        return 1;
    }

    // Pass inum, offset, length, and buffer to the File_Write function
    // to update the inode and write to log
    File file;
    status = file.File_Write(inum, offset, length, buffer);

    if (status > 0)
    {
        std::cerr << "Error: Unable to write to a file" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_write" << std::endl;
        return 1;
    }
    
    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_read
 *
 * Parameters:
 * const char* path - file path
 * void* buffer - a buffer that holds contents to be written to file
 * off_t offset - offset of .ifile
 * size_t length - length in bytes to be written to the log
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * Given the inum of inode, it calls File_Write to initialize the inode's direct/indirect pointers.
 *********************************************************************
 */
int Directory::Directory_file_read(const char* path, void* buffer, off_t offset, size_t length)
{
    char* ch_path = strdup(path);
    char* dirname = basename(ch_path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(dirname, path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inode" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_read" << std::endl;
        return 1;
    }

    u_int inum;
    status = inode.Inode_Get_Inum(inum);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inum of inode" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_read" << std::endl;
        return 1;
    }

    if (inum < 2)
    {
        std::cerr << "Error: Invalid inum. Retrieved inum < 2." << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_read" << std::endl;
        return 1;
    }
    
    if (inum < sizeof(Inode::Container))
    {
        std::cerr << "Error: inum cannot be < size of one inode " << inum << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_read" << std::endl;
        return 1;
    }

    File file;
    status = file.File_Read(inum, offset, length, buffer);
    if (status > 0)
    {
        std::cerr << "Error while reading a file in Directory_file_read" << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_read" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_rename
 *
 * Parameters:
 * const char* org_path - original file path
 * const char* new_path - Desired new path 
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * With the new path name, find the current (or target) file that the
 * user wishes to rename and update the inode's meta data
 *********************************************************************
 */
int Directory::Directory_file_rename(const char* org_path,  const char* new_path)
{
    char* ch_org_path = strdup(org_path);
    char* org_name = basename(ch_org_path);
    char* ch_new_path = strdup(new_path);
    char* new_name = basename(ch_new_path);

    Inode inode;
    int status = inode.Inode_Find_Inode(org_name, org_path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inode with the given path " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_rename" << std::endl;
        return 1;
    }

    status = inode.Inode_Rename(new_name, new_path);
    if (status > 0)
    {
        std::cerr << "Error: Unable to rename the file " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_file_rename" << std::endl;
        return 1;
    }

    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_chmod
 *
 * Parameters:
 * const char* path - file or directory path
 * mode_t mode - a new mode that the user wants to set
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * With a given path and mode, modify the current mode of the file or
 * directory with the new mode.
 *********************************************************************
 */
int Directory::Directory_chmod(const char* path, mode_t mode)
{
    char* ch_path = strdup(path);
    char* name = basename(ch_path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(name, path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inode with the given path " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_chmod" << std::endl;
        return 1;
    }

    status = inode.Inode_Chmod(&inode, mode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to change the mode of the file " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_chmod" << std::endl;
        return 1;
    }
    
    // Add the inode at the back of the list, which will be added to the .file at checkpoint 
    list_of_inodes.push_back(inode); 
    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_chown
 *
 * Parameters:
 * const char* path - file or directory path
 * uid_t uid - new owner's id
 * gid_t gid - new group id
 *
 * Returns:
 *  0 on success, 1 otherwise
 *
 * With a given path, uid, and gid, modify the current mode of the file
 * or directory's uid and gid
 *********************************************************************
 */
int Directory::Directory_chown(const char* path, uid_t uid, gid_t id)
{
    char* ch_path = strdup(path);
    char* name = basename(ch_path); 

    Inode inode;
    int status = inode.Inode_Find_Inode(name, path, &inode);
    if (status > 0)
    {
        std::cerr << "Error: Unable to retrieve the inode with the given path " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_chown" << std::endl;
        return 1;
    }

    status = inode.Inode_Chown(&inode, uid, id);
    if (status > 0)
    {
        std::cerr << "Error: Unable to change uid and gid of the file " << std::endl;
        std::cerr << "File: directory.cpp. Function: Directory_chown" << std::endl;
        return 1;
    }
    
    // Add the inode at the back of the list, which will be added to the .file at checkpoint 
    list_of_inodes.push_back(inode);
    return 0;
}

/*
 *********************************************************************
 * int
 * Directory_file_getattr
 *
 * Parameters:
 * const char* path - file or directory path
 * struct stat* stbuf - a "stat" structure for system call
 *
 * Returns:
 *  
 * struct stat* stbuf - a "stat" structure for system call
 *********************************************************************
 */
int Directory::Directory_file_getattr(const char* path, struct stat* stbuf)
{
    File file;
    Inode file_inode;
    int status = file.File_Getattr(path, stbuf);
    if (status > 0)
    {
        if (list_of_inodes.empty())
        {
            std::cerr << "ifile is empty. No file to retrieve." << std::endl;
            std::cerr << "File: directory.cpp. Function: Directory_file_getattr" << std::endl;
            return 1;
        }
        else
        {
            Inode inode = list_of_inodes.back();
            
            std::list<Inode>::iterator iter;
            for (iter = list_of_inodes.begin(); iter != list_of_inodes.end(); iter++)
            {
                if(strcmp(iter->container.m_path, path) == 0)
                {
                    file_inode = *iter; 
                    status = 0;
                }
            }
            
        }
        if (status > 0)
        {
            std::cerr << "Unable get file attributes " << std::endl;
            std::cerr << "File: directory.cpp. Function: Directory_file_getattr" << std::endl;
            return -ENOENT;
        }
    }
    file_inode.Inode_Set_Stbuf(stbuf, file_inode);
    return 0;
}

int Directory::Directory_file_free(const char* path)
{
    return 0;
}
