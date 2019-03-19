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
// u_int tailSegAddr;

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

u_int num_bytes_in_segment;
u_int num_sectors_in_segment;
u_int num_bytes_in_block;

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

	rc = Log_Open(true);
	if(rc != 0){
		cout << "Error loading log" << endl;
		return rc;
	}

	// Create an empty filesystem which contains only the root directory and the
	// '.', '..', and '.ifile' entries.

	// directory.Create();	//EDIT

	rc = Log_Close();
	if(rc != 0){
		cout << "Error closing log" << endl;
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

	// else load the segment in the cache (not in the position of the tail segment)
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

		int cacheIndexToLoad = lastLoadedCacheIndex + 1;

		if(lastLoadedCacheIndex == -1){
			cacheIndexToLoad = 1;
		}
		if(lastLoadedCacheIndex == NUM_SEGS_IN_CACHE-1){
				cacheIndexToLoad = 1;
		}

		// Assumption is that the memory for all the segment cache is initialized in the beginning
		Segment *segment = &segmentCache[cacheIndexToLoad]
		loadSegment(this_seg, segment);

		lastLoadedCacheIndex = cacheIndexToLoad;

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

	// segmentCache = (Segment*) malloc(NUM_SEGS_IN_CACHE * sizeof(Segment));
	segmentCache = (Segment*) calloc(NUM_SEGS_IN_CACHE, sizeof(Segment));
	if(segmentCache == NULL){
		cout << "Memory allocation for segment cache failed" << endl;
		exit(1);
	}

	// for each segment in the cache, allocate memory for each segment
	for(int i=0; i<NUM_SEGS_IN_CACHE; i++){
		// segmentCache[i].seg_bytes = malloc(superBlock.segment_size * superBlock.block_size
			// * FLASH_SECTOR_SIZE);
		segmentCache[i].seg_bytes = calloc(1, superBlock.segment_size * superBlock.block_size
			* FLASH_SECTOR_SIZE);
		if(segmentCache[i].seg_bytes == NULL){
			cout << "Memory allocation for segment cache failed" << endl;
			exit(1);
		}

		SegSummary thisSegSummary = segmentCache[i].segSummary;

		// thisSegSummary.blockInfos = (BlockInfo*) malloc(superBlock.segment_size * sizeof(BlockInfo));
		thisSegSummary.blockInfos = (BlockInfo*) calloc(superBlock.segment_size, sizeof(BlockInfo));
		if(thisSegSummary.blockInfos == NULL){
			cout << "Memory allocation for segment cache failed" << endl;
			exit(1);
		}

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
 * Updates the segment summary information with the inum of the file 
 * and the block offset of the file (if the block is of type file data)
 *
 *********************************************************************
 */

int Log_Write(u_int inum, u_int block, u_int length, void *buffer, LogAddress *logAddress){

	// check if the tail segment gets full when writing length bytes
	// write the given bytes to the log
	// when the tail segment is full, write the segment to flash
	// and initialize new tail segment

	// compute the number of blocks needed by the write
	int num_blocks = ceil(length / (FLASH_SECTOR_SIZE * superBlock.block_size));

	if(numUsedBlocksInTail + num_blocks >= superBlock.segment_size){
		// the bytes dont fit in the tail segment

		// write the blocks that fit to the tail segment
		// write the tail segment to flash
		// update the next segment field in the tail segment's summary with the new segment's address

		// initialize new tail segment
		// write the remaining blocks to the new segment
		//  update num used blocks in the tail

		// write the blocks that fit to the tail segment
		int num_fit_blocks = superBlock.segment_size - numUsedBlocksInTail;

		u_int fit_length = (u_int) num_fit_blocks * num_bytes_in_block;
		writeToTail(inum, block, fit_length, buffer, logAddress);

		// write the tail segment to flash
		// update the next segment field in the tail segment's summary
		// initialize new tail segment
		// set num used blocks in the tail to 0
		writeTailSegToFlash();

		// write the remaining blocks to the new tail segment
		LogAddress new_logAddress; 
		int remaining_blocks = num_blocks - num_fit_blocks;
		u_int rem_length = (u_int) remaining_blocks * num_bytes_in_block;
		// TODO: When the write spans multiple segments, how do we return the new segment's address
		// in the logAddress
		writeToTail(inum, block, rem_length, (char*)buffer + fit_length, &new_logAddress);
		//  update num used blocks in the tail
		numUsedBlocksInTail += remaining_blocks;

	} else {
	
		// write the blocks to the tail segment
		writeToTail(inum, block, length, buffer, logAddress);
		// update num used blocks in tail
		numUsedBlocksInTail += num_blocks;
	}

	return 0;

}

// update the next segment field in the tail segment's summary
// write the tail segment to flash
// initialize new tail segment
// set num used blocks in the tail to 0
int writeTailSegToFlash(){

	// update the next segment field in the tail segment's summary
	Segment tailSegment = segmentCache[tailSegIndex];
	// set the next segment address to 1 plus the current segment address
	
	u_int next_segment = tailSegment.segSummary.this_segment + 1;
	tailSegment.segSummary.next = next_segment;

	// TODO: Set the next segment address to the address returned by the segment cleaner
	// It is the segment cleaner's job to provide the next available clean segment
	// tailSegment.segSummary.next = segmentCleaner.getNext(tailSegment.segSummary.this_segment);
	
	u_int this_segment = tailSegment.segSummary.this_segment;

	// write the segment summary to the first block of segment or as needed by the size of 
	// segment summary
	SegSummary thisSegSummary = tailSegment.segSummary;

	memcpy(tailSegment.seg_bytes, &thisSegSummary, sizeof(SegSummary));
	memcpy(((char*)tailSegment.seg_bytes) + sizeof(SegSummary), thisSegSummary.blockInfos, 
		sizeof(BlockInfo) * superBlock.segment_size);

	// write the tail segment to Flash
	int rc = Flash_Write(flash, this_segment * num_sectors_in_segment, num_sectors_in_segment,
		tailSegment.seg_bytes);
	if(rc != 0){
		cout << "Error writing tail segment " << this_segment << " to flash"<<endl;
		return rc; 
	}

	// initialize new tail segment
	memset(tailSegment.seg_bytes, 0, num_bytes_in_segment);
	memset(thisSegSummary.blockInfos, 0, superBlock.segment_size * sizeof(BlockInfo));
	thisSegSummary.this_segment = next_segment;
	thisSegSummary.next = thisSegSummary.this_segment + 1;

	// set num used blocks in the tail to 0
	numUsedBlocksInTail = 0;

	
	return 0;

}


// Writes length block in the tail segment and updates the segment summary for the blocks
// involved
// Return 0 on success
int writeToTail(u_int inum, u_int block, u_int length, void *buffer, LogAddress *logAddress){

	//writes length block in the tail segment
	memcpy(((char*)segmentCache[tailSegIndex].seg_bytes) + (numUsedBlocksInTail*num_bytes_in_block), 
		buffer, length);

	// update the logAddress with the address where the memory was written 
	Segment tailSegment = segmentCache[tailSegIndex];
	logAddress->segment = tailSegment.segSummary.this_segment;
	logAddress->block = numUsedBlocksInTail;

	//update the segment summary for the blocks 

	// compute the number of blocks needed by the write
	int num_blocks = ceil(length / num_bytes_in_block);

	// get the starting and ending block of the log
	int start_block = numUsedBlocksInTail;
	int end_block = numUsedBlocksInTail + num_blocks - 1;

	// TODO: Check if inum is valid; If it is valid then this write is part of a file data write
	// Set the type of the block (in segment summary) to file data type if the inum is valid
	//

	if(inum == 0 && block == 0){
		int type = BLKTYPE_OTHER;
	}

	for(int i=start_block; i<=end_block; i++){
		tailSegment.segSummary.blockInfos[i].inum = inum;
		tailSegment.segSummary.blockInfos[i].block_offset = block;
		tailSegment.segSummary.blockInfos[i].type = type;
	}

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

// Opens the flash file and loads the log structures in memory
// If the flash is empty (just created), isFlashEmpty is set to true and
// the log is loaded without a checkpoint. 
// Otherwise the log is loaded using the checkpoint read from flash.

int Log_Open(bool isFlashEmpty=false){

	int rc;

	char *filename;

	if(!isFlashEmpty){
		// Use the filename supplied by lfs
		filename = flash_filename; 
	}	else {
		// Flash memory is just created by mklfs
		// superblock is already loaded
		// Use the filename inside the superblock
		filename = superBlock.fileName;
	}

	// open the flash file 
	Flash_Flags flags = FLASH_SILENT;

	u_int get_num_erase_blocks;
	flash = Flash_Open(filename, flags, &get_num_erase_blocks);
	if (flash == NULL){
		rc = 1;
		cout << "Error opening flash" << endl;
		return rc;
	}

	if(!isFlashEmpty){
		//flash is not empty. This function is called by lfs 
		rc = loadCheckpoint();
		if(rc != 0){
			cout << "Could not load checkpoint from flash" << endl;
			return rc;
		}

	} else {
		// flash just created by mklfs
		// Initialize the log layer without any checkpoint
		rc = loadCheckpoint(true);
		if(rc != 0){
			cout << "Could not initialize log structures" << endl;
			return rc;
		}
	}

	// initialize these values
	num_bytes_in_block = FLASH_SECTOR_SIZE * superBlock.block_size;
	num_bytes_in_segment = num_bytes_in_block * superBlock.segment_size;
	num_sectors_in_segment = superBlock.segment_size * superBlock.block_size;

	// allocate memory for the segment cache
	allocateSegmentCache();

	// The tail segment is always stored in the first position in the segment cache array
	tailSegIndex = 0;
	numUsedBlocksInTail = 0;

	if(!isFlashEmpty){
		int latestIndex;
		// find the latest checkpoint
		if (cr_array[0].write_time > cr_array[1].write_time){
			latestIndex = 0;
		}	else {
			latestIndex = 1;
		}

		// get the last segment written in the checkpoint
		int last_seg_num = cr_array[latestIndex].lastSegmentAddress;
		
		// The tail segment is 1 plus the last segment written in the latest checkpoint
		segmentCache[tailSegIndex].segSummary.this_segment = last_seg_num + 1;

		// loadTailSegmentinCache(); //*** If supporting partially written tail segments,
			// the partially written tail segment from the checkpoint is loaded back into memory and
			// used till its full 
		
	}	else {
		// Start storing the data in the 2nd sector. First sector is reserved for superblock data. 
		segmentCache[tailSegIndex].segSummary.this_segment = 1;
	}

	return rc;

}

// This function writes the checkpoint to the flash,
// closes the flash file and frees all the memory
int Log_Close(){
	int rc = 0;
	
	rc = writeCheckpoint();
	if(rc != 0){
		cout << "Could not write checkpoint to flash" << endl;
		return rc;
	}

	// free memory
	
	// for each segment in the cache, deallocate memory
	for(int i=0; i<NUM_SEGS_IN_CACHE; i++){
		free(segmentCache[i].seg_bytes);
		SegSummary thisSegSummary = segmentCache[i].segSummary;
		free(thisSegSummary.blockInfos);
	}

	free(segmentCache);

	// close the flash file
	rc = Flash_Close(flash);

	if(rc != 0){
		cout << "Error closing flash" << endl;
		return rc;
	}
	return rc;
}

// This function writes the checkpoint to the flash i.e the checkpoint region and updates the address of 
// the checkpoint region in the superblock. 

// Returns 0 on success, 1 otherwise
int writeCheckpoint(){
	// rc is the return code
	int rc = 0;

	// write the current tail segment to flash
	writeTailSegToFlash();

	// prepare the checkpoint and write it to flash on the new segment

	// alternate between the two checkpoints (0 and 1)
	u_int indexToWrite = last_CR_index+1;
	if(indexToWrite == 2){ indexToWrite = 0;}

	cr_array[indexToWrite].write_time = time(0);
	cr_array[indexToWrite].lastSegmentAddress = segmentCache[tailSegIndex].segSummary.this_segment;
	// Setting this address to 0. When loading, treat this as an unassigned or invalid index
	cr_array[indexToWrite].segUsgTblAddress = 0;

	// write the checkpoint region in the tail segment 
	size_t length = sizeof(CR);
	// void* checkpt_bytes = malloc(length);
	// memcpy(checkpt_bytes, &cr_array[indexToWrite], length);

	LogAddress new_logAddress;
	int num_blocks = ceil(length / (FLASH_SECTOR_SIZE * superBlock.block_size));
	// writeToTail(0, 0, length, checkpt_bytes, &new_logAddress);
	writeToTail(0, 0, length, &cr_array[indexToWrite], &new_logAddress);

	// free (checkpt_bytes);

	// update num used blocks in the tail
	numUsedBlocksInTail += num_blocks;

	// write the tail segment to the flash
	writeTailSegToFlash();

	// update the superblock's cr address with the address of the checkpoint
	superBlock.cr_addresses[indexToWrite] = cr_array[indexToWrite].lastSegmentAddress;

	// write the superblock in the first segment
	size_t length = sizeof(SuperBlock);
	rc = Flash_Write(flash, 0, ceil(length / FLASH_SECTOR_SIZE) , &superBlock);
	if(rc != 0){
		cout << "Error writing superblock to flash" << endl;
		return rc;
	}

	last_CR_index = indexToWrite;

	return rc;
}

// This function loads the checkpoint from the flash. It is called by the Log_Open function to initialize 
// the structures in memory

// Returns 0 on success, 1 otherwise
int loadCheckpoint(bool isFlashEmpty=false){
	int rc = 0;

	if(!isFlashEmpty){
	// read the contents of the superblock, and checkpoint information
	// (which contains the inode of the ifile and the last written segment)
		size_t length = sizeof(SuperBlock);
		rc = Flash_Read(flash, 0, ceil(length/FLASH_SECTOR_SIZE), &superBlock);
		if(rc != 0){
			cout << "Error reading superblock from flash" << endl;
			return rc;
		}

		if(superBlock.cr_addresses[0]!=0){
			// load into cr_array
			length = sizeof(CR);
			rc = Flash_Read(flash, superBlock.cr_addresses[0]*num_sectors_in_segment, 
				ceil(length/FLASH_SECTOR_SIZE), &cr_array[0]);
			if(rc != 0){
				cout << "Error reading checkpoint 0 from flash" << endl;
				return rc;
			}

		}

		if(superBlock.cr_addresses[1]!=0){
			// load into cr_array
			length = sizeof(CR);
			rc = Flash_Read(flash, superBlock.cr_addresses[1]*num_sectors_in_segment, 
				ceil(length/FLASH_SECTOR_SIZE), &cr_array[1]);
			if(rc != 0){
				cout << "Error reading checkpoint 1 from flash" << endl;
				return rc;
			}
		}

	}	else {
		// This flash is freshly created
		// clear the checkpoint addresses in the superblock
		superBlock.cr_addresses[0] = 0;
		superBlock.cr_addresses[1] = 0;

		// clear the checkpoint regions
		cr_array[0].write_time = 0;
		cr_array[1].write_time = 0;

		cr_array[0].lastSegmentAddress = 0;
		cr_array[1].lastSegmentAddress = 0;

		cr_array[0].segUsgTblAddress = 0;
		cr_array[1].segUsgTblAddress = 0;

	}

	return rc;

}

