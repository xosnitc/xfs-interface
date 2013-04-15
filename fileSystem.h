#ifndef FILESYS_H
#define FILESYS_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "disk.h"
/*
Declarations of Disk parameters
*/

#define BLOCK_SIZE 512
#define WORD_SIZE 16
#define OS_STARTUP_CODE 0
#define EX_HANDLER 1
#define TIMERINT 3
#define INT1 5
#define INT2 7
#define INT3 9
#define INT4 11
#define INT5 13
#define INT6 15
#define INT7 17

#define OS_STARTUP_CODE_SIZE 1
#define EX_HANDLER_SIZE 2
#define TIMERINT_SIZE 2
#define INT1_SIZE 2
#define INT2_SIZE 2
#define INT3_SIZE 2
#define INT4_SIZE 2
#define INT5_SIZE 2
#define INT6_SIZE 2
#define INT7_SIZE 2

#define FAT 19
#define NO_OF_FAT_BLOCKS 1

#define DISK_FREE_LIST 20
#define NO_OF_FREE_LIST_BLOCKS 1

#define INIT_BASIC_BLOCK 21
#define INIT_NAME "init.xsm"
#define NO_OF_INIT_BLOCKS 3

#define NO_OF_INTERRUPTS 7

#define DATA_START_BLOCK 24
#define NO_OF_DATA_BLOCKS 424

#define SWAP_START_BLOCK 448
#define NO_OF_SWAP_BLOCKS 64

#define NO_OF_DISK_BLOCKS 512

#define DISK_SIZE (NO_OF_DISK_BLOCKS * BLOCK_SIZE)


/*
Declarations for FAT Entry
*/ 
#define FATENTRY_FILENAME 0
#define FATENTRY_FILESIZE 1
#define FATENTRY_BASICBLOCK 2
#define FATENTRY_SIZE 8
#define FAT_SIZE (NO_OF_FAT_BLOCKS * BLOCK_SIZE)

/*
Declarations for files
*/
#define SIZE_EXEFILE_BASIC 4
#define SIZE_EXEFILE 3
#define MAX_DATAFILE_SIZE_BASIC 257
#define MAX_DATAFILE_SIZE 256


/*
Other declarations
*/

#define NO_BLOCKS_TO_COPY 21        //Rest of the blocks have data. Blocks 0-12 need to be copied 
#define EXTRA_BLOCKS	1			// Need a temporary block
#define TEMP_BLOCK 21				//Temporary block no: starting from 0.

#define ASSEMBLY_CODE 0
#define DATA_FILE 1

typedef struct{
	char word[BLOCK_SIZE][WORD_SIZE];
}BLOCK;

BLOCK disk[NO_BLOCKS_TO_COPY + EXTRA_BLOCKS];			// disk contains the memory copy of the necessary blocks of the actual disk file.




/*
  This function lists all the files present on the disk.
*/
void listAllFiles();

/*
  This function deletes an executable file from the disk.
*/
int deleteExecutableFromDisk(char *name);

/*
  This function removes the fat entry corresponding to the first arguement.
*/
int removeFatEntry(int locationOfFat);

/*
  This function returns the basic block entry(pass by pointer) corresponding to the address specified by the second arguement.
*/
int getDataBlocks(int *basicBlockAddr, int locationOfFat);

/*
  This function loads the executable file corresponding to the first arguement to an appropriate location on the disk.
*/
int loadExecutableToDisk(char *name);

/*
  This function checks if a file having name as the first arguement is present on  the disk file.
*/
int CheckRepeatedName(char *name);

/*
  This function returns the address of a free block on the disk.
*/
int FindFreeBlock();

/*
  This function returns an  empty fat entry if present.
*/
int FindEmptyFatEntry();

/*
  This function frees the blocks specified by the block numbers present in the first arguement. The second arguement is the size
  of the first argument.
*/
void FreeUnusedBlock(int *freeBlock, int size);

/*
  This function adds the name, size and basic block address of the file to corresponding entry in the fat.
*/
void AddEntryToMemFat(int startIndexInFat, char *nameOfFile, int sizeOfFile, int addrOfBasicBlock);

/*
  This function copies the necessary contents of a file to the corresponding location specified by the second arguemnt on the disk. The type specifies the type of file 
  to be copied.
*/
int writeFileToDisk(FILE *f, int blockNum, int type);

/*
  This function loads the OS startup code specified by the first arguement to its appropriate location on disk.
*/
int loadOSCode(char* name);

/*
  This function copies the interrupts to the proper location on the disk.
*/
int loadIntCode(char* name, int intNo);

/*
  This function copies the timer interrupt to the proper location on the disk.
*/
int loadTimerCode(char* name);

/*
  This function copies the exception handler to the proper location on the disk.
*/
int loadExHandlerToDisk(char* name);

/*
  This function copies the init program to its proper location on the disk.
*/
int loadINITCode(char* name);

/*
  This function displays the content of the files stored in the disk.
*/
void displayFileContents(char *name);

/*
  This function copies the contents of the disk starting from <startBlock> to <endBlock> to a unix file.
*/
void copyBlocksToFile (int startBlock,int endBlock,char *name);

/*
  This function deletes the INIT code from the disk.
*/
int deleteINITFromDisk();

/*
  This function deletes the OS code from the disk.
*/
int deleteOSCodeFromDisk();

/*
  This function deletes the Timer Interrupt from the disk.
*/
int deleteTimerFromDisk();

/*
  This function deletes the Interrupt <intNo> from the disk.
*/
int deleteIntCode(int intNo);	

/*
  This function deletes the Exception Handler from the disk.
*/
int deleteExHandlerFromDisk();

/*
  This function displays disk free list and the amount of free space in the disk.
*/
void displayDiskFreeList();

/*
  This function loads a data file to the disk.
*/
int loadDataToDisk(char *name);

/*
  This function deletes a data file from the disk.
*/
int deleteDataFromDisk(char *name);

/*
  This function expands environment variables in path
*/
void expandpath(char *path);

/*
  This function adds extensions (.dat or .xsm) for files loaded into xfs
*/
void addext(char *filename, char *ext);
#endif
