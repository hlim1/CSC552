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

#include "flash.h"
#include "file.h"

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

int Log_Write(u_int inum, u_int block, u_int length, void *buffer, LogAddress *logAddress);

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
	char* fileName;		// name of the virtual flash file
	u_int segment_size;	// size of the segment in blocks
	u_int block_size;	// size of the block in sectors 
	u_int num_segments;	// number of segments in flash
	u_int wearLimit;	// wear limit of erase blocks
	SegUsgTbl segUsgTbl;	// segment usage table
	// CR cr;				// checkpoint region

	u_int* cr_addresses; // Since the checkpoint needs to be updated with each file write,
						// this cannot be stored in the starting segments. It will increase
						// the wear level of the blocks. The checkpoint regions also need
						// to be stored at the tail of the log but the segments containing
						// the checkpoint must be recorded in the superblock.
						// Use two checkpoint regions (CR) and use the most recent valid 
						//  of the two to restore the filesystem. 

} SuperBlock;

// Segment Usage Table contatins information about the blocks in each segment
// such as the liveness of the block. This should also keep track of the unused
// blocks in the tail segment created by checkpointing.
typedef struct SegUsgTbl {

} SegUsgTbl;

// Checkpoint region contains the collection of imaps (inode maps)
 typedef struct CR {
	IMaps* imaps;
} CR;

// One or more blocks at the begining of each segment
// Holds per-segment metadata: information about the blocks in the segment
// This should also keep track of the unused blocks in the tail segment created 
// by checkpointing.
typedef struct SegSummary {
	u_int next;	// This gives the next segment in the flash

	u_int *deadBlocks; // This is used by the tail segment to track dead blocks and 
					   // reuse it for new blocks
					   // Blocks may die after they are placed in the tail segment but 
						// before writing it to Flash.
} SegSummary;

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
// N for the N most recently accessed segments
u_int NUM_SEGMENTS_IN_MEM;

void *segmentCache[NUM_SEGMENTS_IN_MEM];

// This stores the segment offset for segments in the segment cache
int segmentCacheOffsets[NUM_SEGMENTS_IN_MEM];

// To allocate memory for each segment
// segmentCache[i] = (void *) malloc(superBlock->segment_size * superBlock->block_size); 

// This stores the tail log segment in memory. This is also stored in the segmentCache.
void *tailSegment;

// The tail segment is written to flash only when it is full. Segment summary provides 
// the information when this segment is full.
SegSummary tailSummary;

void *headSegment;

//TODO: Applications may read blocks from the tail segment before it is written to the Flash
// We need to avoid this.

SuperBlock superBlock;

// This function initializes the log structure and is called by mklfs
// Returns 0 on success, 1 otherwise
int Log_Create();

#endif