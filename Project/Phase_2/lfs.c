#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif

#include <fuse.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <iostream>

#include "log.h"

int main(int argc, char *argv[]){

	// LFS process has the following command-line options.

	// lfs [options] file mountpoint

    // '-f': pass -f to fuse_main so it runs in the foreground
    // -s num, --cache = num: NUM_SEGS_IN_CACHE in log layer i.e. 
    //      size of the cache in log layer in segments, Default 4

    // -i num, --interval = num: checkpoint interval in segments. Default 4
    // -c num, --start=num: threshold at which cleaning starts, in segments: Default 4
    // -C num, --stop=num: threshold at which cleaning stops, in segments: Default 8

    // file: name of the virtual flash file
    // mountpoint: directory on which the LFS filesystem should be mounted

    // ***************************
    // Initialize with the default values
    fuse_f = 0;
    NUM_SEGS_IN_CACHE = 4;
    checkpt_interval = 4;
    clean_start_threshold = 4; 
    clean_stop_threshold = 8;

    int opt=0;

    static struct option long_options[] = {
        {"cache", required_argument, 0, 's'},
        {"interval", required_argument, 0, 'i'},
        {"start", required_argument, 0, 'c'},
        {"stop", required_argument, 0, 'C'},
        {0, 0, 0, 0}
    };

    int long_index = 0;

    while((opt = getopt_long(argc, argv, "s:i:c:C:f",
        long_options, &long_index)) != -1){

        switch(opt) {
            case 'f':
                fuse_f = 1;
                break;

            case 's':
                NUM_SEGS_IN_CACHE = atoi(optarg);
                break;

            case 'i':
                checkpt_interval = atoi(optarg);
                break;

            case 'c':
                clean_start_threshold = atoi(optarg);
                break;

            case 'C':
                clean_stop_threshold = atoi(optarg);
                break;

            default:
                printf("Usage: lfs [options] file mountpoint");
                exit(EXIT_FAILURE);
                break;
        }
    }

    // name of the virtual flash file
    strcpy(flash_filename, argv[argc - 2]);
    // directory of LFS mounting
    strcpy(mountpoint, argv[argc - 1]);

    std::cout << "-f: " << fuse_f<< std::endl;
    std::cout << "cache: " << NUM_SEGS_IN_CACHE<<std::endl;
    std::cout << "interval: " << checkpt_interval<<std::endl;
    std::cout << "start: " << clean_start_threshold<<std::endl;
    std::cout << "stop: " << clean_stop_threshold<<std::endl;
    std::cout << flash_filename << std::endl;
    std::cout << mountpoint << std::endl;

    argv_0 = argv[0];

    Log_Open();

}
