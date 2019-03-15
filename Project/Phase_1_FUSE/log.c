#include "log.h"
// #include "directory.h"
// #include "file.h"

SuperBlock superBlock;

SegUsgTbl segUsgTbl;

u_int last_CR_index = 0;
CR cr_array[2];

// The address of the tail segment
u_int tailSegAddr;

// This tracks the number of blocks that are in use in the tail segment (in memory)
// When all the blocks in the tail segment are used, the segment is written at once
// to the Flash
u_int numUsedBlocksInTail;

// The index of the tail segment in the segment cache
u_int tailSegIndex;

// Array of NUM_SEGS_IN_CACHE segments 
Segment *segmentCache;

// map from segment number to cache index
std::map<int, int> segToCacheMap;

// The smallest unit to read or write data is a segment.
// The segment cache stores in memory the N most recently accessed segments.
u_int NUM_SEGS_IN_CACHE;
u_int checkpt_interval; // checkpoint interval in segments
u_int clean_start_threshold; // threshold at which cleaning starts, in segments
u_int clean_stop_threshold; // threshold at which cleaning stops, in segments

int fuse_f; // If 1, pass the -f argument to fuse_main so it runs in the foreground

char flash_filename[50]; // name of the virtual flash file
char mountpoint[50]; // directory on which the LFS filesystem should be mounted


// This function initializes the log structure and is called by mklfs
// Returns 0 on success, 1 otherwise

// creates and formats a flash for LFS. It uses Flash_Create to create a flash memory,
// then initializes all the on-flash data structures so that the LFS process can access it
// properly. The initial filesystem is empty, and contains only the root directory and the
// '.', '..', and '.ifile' entries.
int Log_Create(){

	// Flash_Create creates a flash memory in file "file". It will contain "blocks" erase blocks
 	// each of which has a wear limit of "wearLimit". The wear on each block is initially 0,
 	// and each block is initially erased. 

	int rc; 

	int num_erase_blocks = superBlock.block_size * 
		superBlock.segment_size * superBlock.num_segments / FLASH_SECTORS_PER_BLOCK;

	rc = Flash_Create(superBlock.fileName, superBlock.wearLimit, num_erase_blocks);

	Flash_Flags flags = FLASH_SILENT;

	u_int get_num_erase_blocks;

	Flash flash = Flash_Open(superBlock.fileName, flags, &get_num_erase_blocks);

	

	// TODO: Create an empty filesystem which contains only the root directory and the
	// '.', '..', and '.ifile' entries.

	// directory.create();	//EDIT

	// TODO: Initialize the SuperBlock with segment usage table and checkpoint regions

	// checkpoint regions

	// TODO: Write the superblock segment in the first segment of flash
	//  and segment summary blocks for each of the segment into the flash memory 

	// TODO: initialize the tail log address

	Flash_Close(flash);

	return rc;

}

// This function opens the virtual flash file and loads the log structures. It is called by lfs.
int Log_Open(){

	return 0;
}

// This function closes the virtual flash file and frees all the memory. Done when unmounting??
int Log_Close(){

	return 0;
}