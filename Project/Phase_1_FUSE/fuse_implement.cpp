#include "fuse_implement.h"

Directory* f_directory;

// Initialize LFS f_directory layer. This will create a root f_directory "/".
int imp_init(struct fuse_conn_info *conn)
{
    std::cout << "Initializing fuse in imp_init" << std::endl;
    f_directory = new Directory();
    return 0;
}

int imp_file_getattr(const char* path, struct stat*)
{
    return NOTSUPPORTED;
}

int imp_file_open(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Opening a file in imp_file_open" << std::endl;
    int status = f_directory->Directory_file_open(path, fi);
    return status;
}

int imp_file_read(const char* path, char* buffer, size_t length, off_t offset, struct fuse_file_info* fi)
{
    std::cout << "File read in imp_file_read" << std::endl;
    int status = f_directory->Directory_file_read(path, buffer, offset, length);
    return status;
}
int imp_file_write(const char* path, char* buffer, off_t offset, size_t length, struct fuse_file_info* fi)
{
    std::cout << "File write in imp_file_write" << std::endl;
    int status = f_directory->Directory_file_write(path, buffer, offset, length);
    return status;
}

int imp_unlink(const char* path)
{
    std::cout << "Unlinking a file from the f_directory. imp_unlink." << std::endl;
    return unlink(path);
}

int imp_rmdir(const char* path)
{
    // NOT IMPLEMENTED YET
    std::cout << "Removing f_directory. imp_rmdir." << std::endl;
    return 0;
}

int imp_statfs(const char* path, struct statvfs* stbuf)
{
    std::cout << "Return statistics about the filesystem. imp_statfs" << std::endl;
    return NOTSUPPORTED;
}

int imp_file_release(const char* path, struct fuse_file_info *fi)
{
    std::cout << "Free up any temporarily allocated data structures when done with a file. file_release" << std::endl;
    int status = f_directory->Directory_file_free(path);
    return status;
}

int imp_mkdir(const char* path, mode_t mode)
{
    std::cout << "mkdir. imp_mkdir" << std::endl;
    int status = f_directory->Directory_create(path, mode);
    return status;
}

int imp_dir_open(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Open f_directory. dir_open" << std::endl;
    int status = f_directory->Directory_open(path, fi);
    return status;
}

int imp_dir_read(const char* path, fuse_fill_dir_t filler, char* buffer, off_t offset, struct fuse_file_info* fi)
{
    std::cout << "Read and return one or more f_directory entries to the caller. imp_dir_read." << std::endl;
    int status = f_directory->Directory_read(path, buffer, filler, offset, fi);
    return status;
}

int imp_dir_release(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Free up any temporarily allocated data structures when done with a f_directory. imp_dir_release" << std::endl;
    int status = f_directory->Directory_free(path);
    return status;
}

void imp_destroy(void *private_data)
{
    std::cout << "Destroy f_directory" << std::endl;
    delete f_directory;
}


int imp_file_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    std::cout << "Create a file. imp_file_create" << std::endl;
    int status = f_directory->Directory_file_create(path, mode, fi);
    return status;
}

int imp_link(const char* src_path, const char* dest_path)
{
    std::cout << "Create a hard link between src_path and dest_path." << std::endl;
    return NOTSUPPORTED;
}

int imp_symlink(const char* src_path, const char* dest_path)
{
    std::cout << "Create a symbolic link named src_path which, when evaluated, will lead to dest_path"
              << std::endl;
    return NOTSUPPORTED;
}

int imp_truncate(const char* path, size_t length)
{
    std::cout << "Truncate or extend the given file so that it is precisely size bytes long. imp_truncate."
              << std::endl;
    int status = f_directory->Directory_file_truncate(path, length);
    return status;
}

int imp_rename(const char* org_path, const char* new_path)
{
    std::cout << "Rename a file. imp_rename" << std::endl;
    int status = f_directory->Directory_file_rename(org_path, new_path);
    return status;
}

int imp_chmod(const char * path, mode_t mode)
{
    std::cout << "Change mode. imp_chmod" << std::endl;
    int status = f_directory->Directory_chmod(path, mode);
    return status;
}

int imp_chown(const char * path, uid_t uid, gid_t id)
{
    std::cout << "Change owner. imp_chown" << std::endl;
    int status = f_directory->Directory_chown(path, uid, id);
    return status;
}
