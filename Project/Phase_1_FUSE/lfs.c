#include "log.h"

int main(int argc, char *argv[]){

	// LFS process has the following command-line options.

	// lfs [options] file mountpoint

    // '-f': pass -f to fuse_main so it runs in the foreground
    // -s num, --cache = num: NUM_SEGMENTS_IN_MEM in log layer i.e. 
    //      size of the cache in log layer in segments, Default 4

    // -i num, --interval = num: checkpoint interval in segments. Default 4
    // -c num, --start=num: threshold at which cleaning starts, in segments: Default 4
    // -C num, --stop=num: threshold at which cleaning stops, in segments: Default 8

    // file: name of the virtual flash file
    // mountpoint: directory on which the LFS filesystem should be mounted

}

