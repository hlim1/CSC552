#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <iostream>

using namespace std;

#include "log.h"
#include "flash.h"

// mklfs creates and formats a flash for LFS. It uses Flash_Create to create a flash memory,
// then initializes all the on-flash data structures so that the LFS process can access it
// properly. The initial filesystem is empty, and contains only the root directory and the
// '.', '..', and '.ifile' entries. 
int main(int argc, char *argv[]){

   // The mklfs command has the following options:
	//mklfs [options] file

	// file is the name of the virtual flash file

	// -b size, --block=size : size of a block, in sectors. Default is 2 (1KB).
	// -l size, --segment=size: segment size, in blocks. Default is 32.
			// size must be a multiple of flash erase block size, report an 
			// error otherwise
	// -s segments, --segments=size: size of the flash in segments. The default is 100.

	// -w limit, --wearlimit=limit: wear limit for erase blocks. default is 1000.

	// return 0 on success, 1 otherwise


	// ***************************
	// Initialize the superblock with the default values
	superBlock.block_size = 2;
	superBlock.segment_size = 32;
	superBlock.num_segments = 100; 
	superBlock.wearLimit = 1000;

	int opt=0;

	static struct option long_options[] = {
		{"block", required_argument, 0, 'b'},
		{"segment", required_argument, 0, 'l'},
		{"segments", required_argument, 0, 's'},
		{"wearlimit", required_argument, 0, 'w'},
		{0, 0, 0, 0}
	};

	int long_index = 0;

	while((opt = getopt_long(argc, argv, "b:l:s:w:",
		long_options, &long_index)) != -1){

		switch(opt) {
			case 'b':
				superBlock.block_size = atoi(optarg);
				break;

			case 'l':
				superBlock.segment_size = atoi(optarg);
				break;

			case 's':
				superBlock.num_segments = atoi(optarg);
				break;

			case 'w':
				superBlock.wearLimit = atoi(optarg);
				break;

			default:
				printf("Usage: mklfs [options] file");
				exit(EXIT_FAILURE);
				break;
		}
	}

	// check if the segment size is the multiple of flash erase block size 
	if((superBlock.segment_size*superBlock.block_size)%FLASH_SECTORS_PER_BLOCK != 0){
		printf("segment size must be a multiple of flash erase block size\n");
		exit(EXIT_FAILURE);
	}

	// This is the filename of the virtual flash file
	// superBlock.fileName = argv[argc - 1];
	strcpy(superBlock.fileName, argv[argc - 1]);

	cout << "block: " << superBlock.block_size<<endl;
    cout << "segment: " << superBlock.segment_size<<endl;
    cout << "segments: " << superBlock.num_segments<<endl;
    cout << "wearlimit: " << superBlock.wearLimit<<endl;
    cout << superBlock.fileName << endl;
    
	return Log_Create();

}

