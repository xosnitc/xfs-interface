#include "createDisk.h"


/*
  createDisk creates  the disk file if not present.
  if format is equal to zero then the function creates the disk but does not format it.
  if format is not equal to zero then the function will create and format the disk.
  Formatting is done as follows:
    1. A memory copy of the disk is maintained. This copy contains NO_BLOCKS_TO_COPY + EXTRA_BLOCKS (in this case 13 + 1) blocks.
      The extra block is a temporary block. This memory copy is called the virtual disk. This is first cleared.
    2. Then the memory freelist is initialised.
    3. The fat blocks are also initialised. The basic block entries are all set to -1. The memory copy is then committed to the 
      disk file.
    4. Finally the entry for init process is made.
*/
void createDisk(int format)
{
    int fd;
    if(format)
    {
		fd = open(DISK_NAME, O_CREAT | O_TRUNC | O_SYNC, 0666);
		clearVirtDisk();
		close(fd);
		int i=0,j=0;
		
		for(j=0; j<(NO_OF_FREE_LIST_BLOCKS*BLOCK_SIZE); j++)
		{
			i=j/BLOCK_SIZE;
			if( (j>=DATA_START_BLOCK) && (j<NO_OF_DISK_BLOCKS ))
				storeValue(disk[DISK_FREE_LIST+i].word[j], 0);
			else
				storeValue(disk[DISK_FREE_LIST+i].word[j], 1);
		}
		
		for(i=0; i<NO_OF_FREE_LIST_BLOCKS;i++)
			writeToDisk(DISK_FREE_LIST+i, DISK_FREE_LIST+i);
			
		for(j=0; j<NO_OF_FAT_BLOCKS; j++)
		{
			for(i=FATENTRY_BASICBLOCK; i<BLOCK_SIZE; i=i+FATENTRY_SIZE)
			{
				storeValue(disk[FAT + j].word[i], -1);
			}
			for(i=FATENTRY_FILENAME; i<BLOCK_SIZE; i=i+FATENTRY_SIZE)
			{
				storeValue(disk[FAT + j].word[i], -1);
			}
			for(i=FATENTRY_FILESIZE; i<BLOCK_SIZE; i=i+FATENTRY_SIZE)
			{
				storeValue(disk[FAT + j].word[i], 0);
			}
			writeToDisk(FAT+j, FAT+j);
		}
	}
	else
	{
		fd = open(DISK_NAME, O_CREAT, 0666);
		close(fd);
	}
	
}


