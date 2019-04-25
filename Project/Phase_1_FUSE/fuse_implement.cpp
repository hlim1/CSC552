#include "fuse_implement.h"

// Initialize LFS directory layer. This will create a root directory "/".
int imp_init(struct fuse_conn_info *conn)
{
    std::cout << "Initializing fuse in imp_init" << std::endl;
    return directory = new Directory(((inputState*)fuse_get_context()->private_data)->lfsFile);
}

int imp_file_getattr(const char* path, struct stat*)
{
    // NOT IMPLEMENTED YET
    return NOTSUPPORTED;
}

int imp_file_open(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Opening a file in imp_file_open" << std::endl;
    return directory->Directory_file_open(path, fi);
}

int imp_file_read(const char* path, char* buffer, int length, off_t offset, struct fuse_file_info* fi)
{
    std::cout << "File read in imp_file_read" << std::endl;
    return directory->Directory_file_read(path, buffer, offset, length);
}

int imp_file_write(const char* path, void* buffer, off_t offset, int length, struct fuse_file_info* fi)
{
    std::cout << "File write in imp_file_write" << std::endl;
    return directory->Directory_file_write(path, buffer, offset, length);
}

int imp_unlink(const char* path)
{
    std::cout << "Unlinking a file from the directory. imp_unlink." << std::endl;
    return unlink(path);
}

int imp_rmdir(const char* path)
{
    // NOT IMPLEMENTED YET
    std::cout << "Removing directory. imp_rmdir." << std::endl;
    return 0;
}

int imp_statfs(const char* path, struct statvfs* stbuf)
{
    std::cout << "Return statistics about the filesystem. imp_statfs" << std::endl;
    return statvfs(path, statvfs);
}

int imp_file_release(const char* path, struct fuse_file_info *fi)
{
    std::cout << "Free up any temporarily allocated data structures when done with a file. file_release"
              << std::endl
    return directory->Directory_file_free(path, fi);
}

int imp_mkdir(const char* path, mode_t mode)
{
    std::cout << "mkdir. imp_mkdir" << std::endl;
    return directory->Directory_Create(path, mode);
}

int imp_dir_open(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Open directory. dir_open" << std::endl;
    return directory->Directory_open(path, fi);
}

int imp_dir_read(const char* path, fuse_fill_dir_t filler, void* buffer, off_t offset, struct fuse_file_info* fi)
{
    std::cout << "Read and return one or more directory entries to the caller. imp_dir_read." << std::endl;
    return directory->Directory_read(path, length, buffer, filler, offset, fi);
}

int imp_dir_release(const char* path, struct fuse_file_info* fi)
{
    std::cout << "Free up any temporarily allocated data structures when done with a directory. im_dir_release" << std::endl;

    return directory->Directory_free(path, fi)
}

int imp_destroy(void *private_data)
{
    std::cout << "Destroy directory" << std::endl;
    delete dictionary;
}


int imp_file_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    std::cout << "Create a file. imp_file_create" << std::endl;
    return directory->Directory_file_create(path, fi);
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

int imp_truncate(const char* path, int size)
{
    std::cout << "Truncate or extend the given file so that it is precisely size bytes long. imp_truncate."
              << std::endl;
    return dictionary->Directory_file_truncate(path, size);
}

int imp_rename(const char* org_path, const char* new_path, unsigned int flags)
{
    std::cout << "Rename a file. imp_rename" << std::endl;
    return Directory->Directory_file_rename(org_path, new_path);
}

int imp_chmod(const char * path, mode_t mode, struct fuse_file_info *fi)
{
    std::cout << "Change mode. imp_chmod" << std::endl;
    return Directory->Directory_chmod(path, mode, fi);
}

int imp_chown(const char * path, uid_t uid, gid_t id, struct fuse_file_info *fi)
{
    std::cout << "Change owner. imp_chown" << std::endl;
    return Directory->Directory_chown(path, uid, id, fi);
}
