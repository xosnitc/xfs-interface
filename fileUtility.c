#include "fileUtility.h"


/*
 This function empties a block as specified by the first arguement in the memory copy of the disk file.
*/
void emptyBlock(int blockNo) 
{
	int i;
	for(i = 0 ; i < BLOCK_SIZE ; i++)
	{
		strcpy(disk[blockNo].word[i],"") ;
	}
}

/*
  char* to int conversion
*/
int getValue(char* str ) 
{
	return atoi(str);
}


/*
  int to char* conversion
*/
void storeValue(char *str, int num) 
{
	sprintf(str,"%d",num);
}


/*
 This function reads an entire BLOCK from the address specified from fileBlockNumber on the disk file to virtBlockNumber on the memory copy of the disk.
*/
int readFromDisk(int virtBlockNumber, int fileBlockNumber) {
	int fd;
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd < 0)
	{
		printf("Unable to Open Disk File\n");
		return -1;
	}
	lseek(fd,sizeof (BLOCK)*fileBlockNumber,SEEK_SET);
	read(fd,&disk[virtBlockNumber],sizeof (BLOCK));
	close(fd);
	return 0;
}


/*
  This function writes an entire block to fileBlocknumber on the disk file from virtBlockNumber on the memory copy of the disk.
*/
int writeToDisk(int virtBlockNumber, int fileBlockNumber) {
	int fd;
	fd = open(DISK_NAME, O_WRONLY, 0666);
	if(fd < 0)
	{
		printf("Unable to Open Disk File\n");
		return -1;
	}
	lseek(fd,0,SEEK_SET);
	lseek(fd,sizeof (BLOCK)*fileBlockNumber,SEEK_CUR);
	write(fd,&disk[virtBlockNumber],sizeof (BLOCK));
	close(fd);	
	return 0;
}


/*
  This function initialises the memory copy of the disk with the contents from the actual disk file.
*/
int loadFileToVirtualDisk()
{
	int i;
	for(i=DISK_FREE_LIST; i<DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)
		readFromDisk(i,i);
	for(i=FAT; i<FAT + NO_OF_FAT_BLOCKS; i++)
		readFromDisk(i,i);
}

/*
  This function wipes out the entire contents of the memory copy of the disk.
*/
void clearVirtDisk()
{
	bzero(disk, sizeof(disk));
}
