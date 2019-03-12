#include <directory.h>

void Directory_create ()
{
    directory.open(".root", std::ios::binary | std::ios::out | std::ios::app);
}

void directory_file_create (const char* path, mode_t mode, uid_t owner, gid_t group)
{

}
