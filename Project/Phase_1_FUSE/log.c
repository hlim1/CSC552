#include <iostream>
#include <math.h>

#include "log.h"

using namespace std;

// #include "directory.h"
// #include "file.h"

// Global Definitions
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
// ******** For simplicity, the tail segment is always in the 0 position in the segmentCache array

std::map<int, int> segToCacheMap;
int lastLoadedCacheIndex = -1;

// The smallest unit to read or write data is a segment.
// The segment cache stores in memory the N most recently accessed segments.
u_int NUM_SEGS_IN_CACHE;
u_int checkpt_interval; // checkpoint interval in segments
u_int clean_start_threshold; // threshold at which cleaning starts, in segments
u_int clean_stop_threshold; // threshold at which cleaning stops, in segments

int fuse_f; // If 1, pass the -f argument to fuse_main so it runs in the foreground

char flash_filename[50]; // name of the virtual flash file
char mountpoint[50]; // directory on which the LFS filesystem should be mounted

// handle for the flash memory
Flash flash;

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

	if (rc != 0){
		// Error creating flash
		cout << "Error creating flash" << endl;
		return rc;
	}

	Flash_Flags flags = FLASH_SILENT;

	u_int get_num_erase_blocks;

	flash = Flash_Open(superBlock.fileName, flags, &get_num_erase_blocks);
	if (flash == NULL){
		rc = 1;
		cout << "Error opening flash" << endl;
		return rc;
	}

	// TODO: Create an empty filesystem which contains only the root directory and the
	// '.', '..', and '.ifile' entries.

	// directory.create();	//EDIT

	// TODO: Initialize the SuperBlock with segment usage table and checkpoint regions

	// TODO: Write the superblock segment in the first segment of flash
	//  and segment summary blocks for each of the segment into the flash memory 

	// TODO: initialize the tail log address

	rc = Flash_Close(flash);

	if(rc != 0){
		cout << "Error closing flash" << endl;
		return rc;
	}

	return rc;

}

/*
 *********************************************************************
 * int
 * Log_Read
 *
 * Parameters:
 *
 * 	LogAddress	logAddress -- log segment and block number within the segment
 *	u_int	length -- # of bytes to read
 *	void	*buffer -- buffer into which log is read
 *
 * Returns:
 *	0 on success, 1 otherwise
 *
 * Log_Read reads "length" bytes from "logAddress" into "buffer"
 *
 *********************************************************************
 */ 

int Log_Read(LogAddress logAddress, u_int length, void *buffer){

	// check if the given segment is in the segment cache

	// if present, retrieve the length bytes from the address
	// and place into the buffer

	// else load the segment in the cache (except in the position of the tail segment)
	// retrieve the length bytes from the address

	u_int this_seg = logAddress.segment;
	u_int this_block = logAddress.block;

	// find this_seg in the cache map
	std::map<int, int>::iterator it = segToCacheMap.find(this_seg);
	if(it != segToCacheMap.end()){
		// this segment is in the cache
		// get the segment from the cache
		int segIndexinCache = it->second;

		Segment *segment = &(segmentCache[segIndexinCache]);

		// retrieve the length bytes from the address
		// and place into the buffer

		rc = readFromSegment(segment, logAddress, length, buffer);

		if(rc !=0 ){
			cout << "Error reading from logAddress " << this_seg << " " << this_block
			<< endl;
			return rc;  
		}



	}	else {
		// Load the new segment into the cache
		// Use a round robin strategy to replace an existing segment
		// Later we can use other strategies such as replacing the least used segment in the cache

		int cacheIndexToLoad;

		if(lastLoadedCacheIndex == -1){
			cacheIndexToLoad = 1;
		}
		if(lastLoadedCacheIndex == NUM_SEGS_IN_CACHE-1){
				cacheIndexToLoad = 1;
		}

		// Assumption is that the memory for all the segment cache is initialized in the beginning
		Segment *segment = &segmentCache[cacheIndexToLoad]
		loadSegment(this_seg, segment);

		// retrieve the length bytes from the address
		// and place into the buffer
		rc = readFromSegment(segment, logAddress, length, buffer);

		if(rc !=0 ){
			cout << "Error reading from logAddress " << this_seg << " " << this_block
			<< endl;
			return rc;  
		}
	}

	return 0;
	

}

// loads this segment from flash into the segment
int loadSegment(u_int segmentNum, Segment* segment){
	int rc;
	u_int sector = segmentNum * superBlock.segment_size * superBlock.block_size; 
	// use flash read to read the required bytes
	rc = Flash_Read(flash, sector, 
		superBlock.segment_size * superBlock.block_size, segment->seg_bytes);

	if(rc != 0){
		cout << "Error loading segment " << segmentNum << endl;
		return rc;
	}

	segment->SegSummary.this_segment = segmentNum;

	// Load the segment summary information from the first block of the segment
	loadSegmentSummary(Segment* segment);

	return rc;
}

// This function loads the segment summary information from the first block of the segment
// into the segment summary structure
int loadSegmentSummary(Segment* segment){

	memcpy(&(segment->segSummary), segment->seg_bytes, sizeof(SegSummary));

	memcpy((segment->segSummary.blockInfos), (SegSummary*)(segment->seg_bytes) + 1, 
		sizeof(BlockInfo)*superBlock.segment_size);

	return 0;

}

// allocate memory for the segmentCache i.e. allocate memory for NUM_SEGS_IN_CACHE segments
int allocateSegmentCache() {
	int rc;

	segmentCache = (Segment*) malloc(NUM_SEGS_IN_CACHE * sizeof(Segment));

	// for each segment in the cache, allocate memory for each segment
	for(int i=0; i<NUM_SEGS_IN_CACHE; i++){
		segmentCache[i].seg_bytes = malloc(superBlock.segment_size * superBlock.block_size
			* FLASH_SECTOR_SIZE);

		SegSummary thisSegSummary = segmentCache[i].segSummary;

		thisSegSummary.blockInfos = (BlockInfo*) malloc(superBlock.segment_size * sizeof(BlockInfo));

	}

	return 0;
}

// This function reads the length bytes into the buffer from the logAddress in the 
// segment (retrieved from cache)

// Flash can be read in whole number of sectors.
// TODO: Ensure that the buffer has enough space to hold the sectors after 
// rounding up to the whole number of sectors.

int readFromSegment(Segment *segment, LogAddress logAddress, u_int length, void *buffer){

	u_int this_seg = logAddress.segment;
	u_int this_block = logAddress.block;

	// calculate the sector offset using the block number and the segment number
	u_int sector_offset = this_seg * superBlock.segment_size * superBlock.block_size;
	sector_offset += this_block * superBlock.block_size;

	u_int count = ceil(length / FLASH_SECTOR_SIZE);

	// calculate the number of sectors needed from the length 
	// read count sectors from the flash
	int rc = Flash_Read(flash, sector_offset, count, buffer);

	if(rc != 0){
		cout << "Error reading logAddress: segment " << this_seg
		<< ", block " << this_block << endl;  
		return rc;
	}

	return rc;
}

/*
 *********************************************************************
 * int
 * Log_Write
 *
 * Parameters:
 *
 * 	u_int	inum -- inode number of the file
 *	u_int	block -- block number within the file
 *	u_int	length -- # of bytes to write
 *  void 	*buffer -- buffer from which log is written
 *	LogAddress	*logAddress -- log segment and block number within the segment
 *
 * Returns:
 *	0 on success, 1 otherwise
 *
 * Log_Write writes "length" bytes from "buffer" into the log.
 * It updates the "logAddress" with the log segment and block number 
 * where the bytes were written.
 * TODO: What is the purpose of inum and block in this function
 *
 *********************************************************************
 */

int Log_Write(u_int inum, u_int block, u_int length, void *buffer, LogAddress *logAddress){

	// check if the tail segment gets full with the writing of these bytes
 	// write the given bytes to the log
	// when the tail segment is full, write the segment to flash
	// and initialize new tail segment

}

/*
 *********************************************************************
 * int
 * Log_Free
 *
 * Parameters:
 *
 * 	LogAddress	logAddress -- log segment and block number within the segment
 *	u_int	length -- # of bytes to free
 *
 * Returns:
 *	0 on success, 1 otherwise
 *
 * Log_Free frees "length" units from "logAddress"
 *
 *********************************************************************
 */

int Log_Free(LogAddress logAddress, u_int length){

	// update the segment usage table to set the length bytes free
	// at the given address	
}

// This function opens the virtual flash file and loads the log structures. It is called by lfs.
int Log_Open(){

	// open the flash file and read the contents of the superblock, and checkpoint information
	// (which contains the inode of the ifile and the last written segment)

	// prepare the next segment to be the tail segment of the block (or use the remaining unused blocks
	// of the last written segment before using the next segment)

	int rc = 0;

	allocateSegmentCache();

	rc = load_checkpoint();
	if(rc != 0){
		cout << "Could not load checkpoint from flash" << endl;
		return rc;
	}

	return rc;
}

// This function closes the virtual flash file and frees all the memory. Done when unmounting??
// If done when unmounting, writes the checkpoint to the flash
int Log_Close(){
	int rc = 0;
	
	rc = write_checkpoint();
	if(rc != 0){
		cout << "Could not write checkpoint to flash" << endl;
		return rc;
	}

	return rc;
}

// This function writes the checkpoint to the flash i.e the checkpoint region and updates the address of 
// the checkpoint region in the superblock. 

// Returns 0 on success, 1 otherwise
int write_checkpoint(){
	// rc is the return code
	int rc = 0;

	return rc;
}

// This function loads the checkpoint from the flash. It is called by the Log_Open function to initialize 
// the structures in memory

// Returns 0 on success, 1 otherwise
int load_checkpoint(){
	int rc = 0;

	return rc;

}

