#include "log.h"

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

	int get_num_erase_blocks;

	Flash flash = Flash_Open(superBlock.filename, flags, &get_num_erase_blocks);

	

	// TODO: Create an empty filesystem which contains only the root directory and the
	// '.', '..', and '.ifile' entries.

	// TODO: Initialize the SuperBlock with segment usage table and checkpoint regions

	// TODO: Write the superblock segment in the first segment of flash
	//  and segment summary blocks for each of the segment into the flash memory 

	// TODO: initialize the tail log address

	

	Flash_Close(flash);

	return rc;

}
