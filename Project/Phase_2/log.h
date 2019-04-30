/* 
**********************************************************************
*
* log.h --
*
* Declarations for the Log layer.
* This layer is above the Flash layer in the LFS hierarchy.
*
**********************************************************************
*/

#ifndef _LOG_H
#define _LOG_H

#ifndef FUSE_USE_VERSION
#define FUSE_USE_VERSION 26
#endif

#include <fuse.h>

#include "flash.h"
// #include "file.h"
#include <ctime>
#include <map>
#include <string.h>

// using namespace std;

// These are types of blocks. Currently there are either file block or other block.
// Other block may be inode block, dir block, or segment usage block
#define BLKTYPE_FILE 0
#define BLKTYPE_OTHER 1 

extern u_int num_bytes_in_segment;
extern u_int num_sectors_in_segment;
extern u_int num_bytes_in_block;

typedef struct LogAddress {
	u_int segment;	// starting offset of the segment in flash 
	u_int block;	// offset of the block (block number) within the segment
} LogAddress;

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

int Log_Read(LogAddress logAddress, u_int length, void *buffer);

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

int Log_Write(u_int inum, u_int block, u_int length, const void *buffer, LogAddress *logAddress);

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

int Log_Free(LogAddress logAddress, u_int length);

// One or more segments at the beginning of the flash
// Contains the lfs metadata
typedef struct SuperBlock {
	char fileName[50];		// name of the virtual flash file
	u_int segment_size;	// size of the segment in blocks
	u_int block_size;	// size of the block in sectors 
	u_int num_segments;	// number of segments in flash
	u_int wearLimit;	// wear limit of erase blocks
	// CR cr;				// checkpoint region

	// u_int cr_addresses[2];
	LogAddress cr_addresses[2];
	 					// Since the checkpoint needs to be updated with each file write,
						// this cannot be stored in the starting segments. It will increase
						// the wear level of the blocks. The checkpoint regions also need
						// to be stored at the tail of the log but the segments containing
						// the checkpoint must be recorded in the superblock.
						// Use two checkpoint regions (CR) and use the most recent valid 
						//  of the two to restore the filesystem. 

} SuperBlock;


extern SuperBlock superBlock;

// Segment Usage Table contains information about the blocks in each segment
// such as the liveness of the block. This should also keep track of the unused
// blocks in the tail segment created by checkpointing.

// From paper, sec 3.6: For each segment, it stores the number of live bytes in the segment and
// the most recent write time of any block in the segment. The values are initialized when the
// segment is written. Live bytes are decremented when the files are deleted or blocks are
// overwritten. 
// The blocks of the segment usage table is stored in the log and the address is stored in 
// the checkpoint region.

typedef struct SegUsage {
	u_int num_live_bytes;	// number of live bytes in the segment
	std::time_t most_recent_modified_time; // most recent modified time of any block in the segment
} SegUsage;


typedef struct SegUsgTbl {
	// Array of segment usages. Its size is the number of segments
	SegUsage *segUsages;
} SegUsgTbl;


extern SegUsgTbl segUsgTbl;

// Checkpoint region contains the collection of imaps (inode maps) and the address of the segment
// usage table

// From paper; Locates blocks of inode maps and segment usage table, stores last checkpoint.
// In a checkpoint, the filesytem is in a consistent and complete state. The CR contains the 
// addresses of all the blocks in the inode map and segment usage table, plus the current time
// and a pointer to the last segment written.

// LFS performs checkpoints at intervals and when the file system is unmounted or on shut down

// This stores the index of the last CR used to create the checkpoint
// LFS alternates between the two checkpoints. This value alternates between 0,1. 
extern u_int last_CR_index;

typedef struct CR {
	// iMap *imaps;	// The collection of imaps is stored in the file called .ifile in the root dir

	// inode ifile_inode;	//EDIT: the inode of the ifile 
	// NOTE: The ifile also needs to be written during checkpointing

	// Instead of storing the inode in the checkpoint, it stores the block address 
	LogAddress inode_ifile_address;

	u_int segUsgTblAddress;	// the address of segment usage table
	time_t write_time;	// time of creating the checkpoint 
	// the time is stored in the last block of the CR (optional detail). If checkpoint fails, 
	// the time will not be updated.
	u_int lastSegmentAddress; // the address of the last segment written

} CR;

// The (two) checkpoint structures are stored here
extern CR cr_array[2];

// One or more blocks at the begining of each segment
// Holds per-segment metadata: information about the blocks in the segment
// This should also keep track of the unused blocks in the tail segment created 
// by checkpointing.

// From paper 3.6; segment summary stores the age of the youngest block written to the segment
// Isn't this also stored in the segment usage table ??

// For each file data block, the summary block contains the file number and block number (offset) for 
// the block.
// Inode map contains the version number for each file and increments when a file is deleted or 
// truncated to zero length. The blocks are given the id which combines the inum and the version 
// number of the file.

typedef struct BlockInfo {
	u_int inum;	// file number of the block
	u_int block_offset;	// offset i.e. block number in the file's inode
	// char uID[20]; //The id combines the inum and the version number of the file.
	int type; // To determine if this block is a file block or inode block or dir block or seg 
	// usage block
} BlockInfo;

typedef struct SegSummary {
	u_int this_segment; // The segment number (offset) of this segment
	u_int next;	// This gives the next segment in the flash

	BlockInfo *blockInfos;	// contains the file number and offset of the block
	// The array size is the number of blocks in a segment.

	// u_int *deadBlocks; // This is used by the tail segment to track dead blocks and 
					   // reuse it for new blocks
					   // Blocks may die after they are placed in the tail segment but 
						// before writing it to Flash.
	

} SegSummary;



// The address of the tail segment
// extern u_int tailSegAddr;

// This tracks the number of blocks that are in use in the tail segment (in memory)
// When all the blocks in the tail segment are used, the segment is written at once
// to the Flash
extern u_int numUsedBlocksInTail;

// The index of the tail segment in the segment cache
extern u_int tailSegIndex;

// This contains the segment summary as well as the 
// contents of the segment
typedef struct Segment {
	void *seg_bytes;	// This contains the (FLASH_SECTOR_SIZE * superBlock.block_size
		// * superBlock.segment_size) bytes
	// To allocate memory for each segment
	// seg_bytes = (void *) malloc(superBlock.segment_size * superBlock.block_size
	// * FLASH_SECTOR_SIZE);

	SegSummary segSummary;
} Segment;

// Array of NUM_SEGS_IN_CACHE segments 
extern Segment *segmentCache;

// map from segment number to cache index
extern std::map<int, int> segToCacheMap;

/*** example map usage ***/
// segToCache.insert(pair<int, int>(40, 1)); 



// This function reads the superblock from the flash into the memory
// Reads "length" number of segments from the "segment" offset in the flash
int readSuperBlock(u_int segment, u_int length);

/***************************
// Creates an empty LFS. Initializes all the on-flash data structures and metadata
// Creates the root directory and its initial contents.
int mklfs();

// Scans the lfs and reports errors such as files that do not have directory entries,
// directory entries that point to unused inodes, wrong segment summary information etc.
// Useful for debugging.
int lfsck();
****************************/

// The smallest unit to read or write data is a segment.
// The segment cache stores in memory the N most recently accessed segments.
extern u_int NUM_SEGS_IN_CACHE;
extern u_int checkpt_interval; // checkpoint interval in segments
extern u_int clean_start_threshold; // threshold at which cleaning starts, in segments
extern u_int clean_stop_threshold; // threshold at which cleaning stops, in segments

extern int fuse_f; // If 1, pass the -f argument to fuse_main so it runs in the foreground

extern char flash_filename[50]; // name of the virtual flash file
extern char mountpoint[50]; // directory on which the LFS filesystem should be mounted

//NOTE: Applications may read blocks from the tail segment before it is written to the Flash
// We need to avoid this.

// This function creates the log structure and formats the virtual flash file. It is called by mklfs.
// Returns 0 on success, 1 otherwise
int Log_Create();

// This function opens the virtual flash file and loads the log structures. It is called by lfs.
int Log_Open(bool isFlashEmpty=false);

// This function closes the virtual flash file and frees all the memory. Done when unmounting??
int Log_Close();

int allocateSegmentCache();
int readFromSegment(Segment *segment, LogAddress logAddress, u_int length, void *buffer);

int loadSegment(u_int segmentNum, Segment* segment);
int loadSegmentSummary(Segment* segment);

int writeTailSegToFlash();
int writeToTail(u_int inum, u_int block, u_int length, const void *buffer, LogAddress *logAddress);

int writeCheckpoint();
int loadCheckpoint(bool isFlashEmpty=false);



extern char *argv_0;

#endif
