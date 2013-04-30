#include "fileSystem.h"
#include<stdio.h>
#include<stdlib.h>



/*
  This function lists all the files present on the disk.
  This is done as follows:
    1. The basic block entry in the memory copy of the disk is searched. If the value is not -1 then the filename is 
      shown as output.
*/
void listAllFiles(){
	int fd;
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd < 0){
	  printf("Unable to Open Disk File!\n");
	  return;
	}
	close(fd);
	int i,j;
	int hasFiles = 0; 	// Flag which indicates if disk has no files
	for(j = FAT ; j < FAT + NO_OF_FAT_BLOCKS ; j++)
	{
		for(i = 0 ; i < BLOCK_SIZE ; i = i + FATENTRY_SIZE)
		{
			if( getValue(disk[j].word[i+FATENTRY_BASICBLOCK]) > 0 )	// Negative value indicates invalid FAT
			{ 	hasFiles = 1;
				printf("Filename: %s   Filesize: %d\n",disk[j].word[i+FATENTRY_FILENAME],getValue(disk[j].word[i+FATENTRY_FILESIZE]));
			}		
		}
	}
	if(!hasFiles)
	{
		printf("The disk contains no files!\n");
	}
}


/*
  This function checks if a file having name as the first arguement is present on the disk file.
  This is done as follows:
    1. It checks the entry in the fat block. If a file with same name exists then the function returns the relative word
      address of the entry having the same name.
*/
int CheckRepeatedName(char *name){
	int i,j;
	for(j = FAT ; j < FAT + NO_OF_FAT_BLOCKS ; j++)
	{
		for(i = FATENTRY_FILENAME ; i < BLOCK_SIZE ; i = i + FATENTRY_SIZE)
		{
			if(strcmp(disk[j].word[i],name) == 0 && getValue(disk[j].word[i]) != -1)		
				return (((j - FAT) * BLOCK_SIZE) + i);
		}
	}
	return (((j - FAT)* BLOCK_SIZE) + i);
}



/*
  This function returns the basic block entry(pass by pointer) corresponding to the address specified by the second arguement.
  Third argument specifies the type of file (assembly code or data file)
  NOTE: locationOfFat - relative word address of the name field in the fat.
*/
int getDataBlocks(int *basicBlockAddr, int locationOfFat)
{
	
	int i,a;
	basicBlockAddr[0] = getValue(disk[FAT + locationOfFat / BLOCK_SIZE].word[locationOfFat % BLOCK_SIZE + FATENTRY_BASICBLOCK]);
	emptyBlock(TEMP_BLOCK);
	//printf("Basic Block = %d\n",basicBlockAddr[0]);
	readFromDisk(TEMP_BLOCK,basicBlockAddr[0]);
	
	i = 0;
	a = getValue(disk[TEMP_BLOCK].word[i]);
	while ((a > 0) && i < MAX_DATAFILE_SIZE)
	{
		//printf("%d %d\t",i+1,a);
		basicBlockAddr[i+1] = a;
		i++;
		a = getValue(disk[TEMP_BLOCK].word[i]);
	}
	
	return 0;
}



/*
  This function frees the blocks specified by the block number present in the first arguement. The second arguement is the size
  of the first argument.
  The memory copy is not committed.
*/
void FreeUnusedBlock(int *freeBlock, int size){
	int i=0;
	for( i = 0 ; i < size && freeBlock[i] != -1 && freeBlock[i] != 0; i++){
		//printf("Block Num = %d\nLocation = %d", freeBlock[i],freeBlock[i] % BLOCK_SIZE );
		storeValue( disk[DISK_FREE_LIST + freeBlock[i] / BLOCK_SIZE].word[freeBlock[i] % BLOCK_SIZE] , 0 );
		emptyBlock(TEMP_BLOCK);
		writeToDisk(TEMP_BLOCK,freeBlock[i]);
	}
}


/*
  This function removes the fat entry corresponding to the first arguement.
  NOTE: locationOfFat - relative word address of the name field in the fat.
  This is done as follows:
    1. The name field is set to empty string.
    2. The basic block entry is set to -1.
  The memory copy is not committed.
*/
int removeFatEntry(int locationOfFat){
	int i;
	int blockNumber = FAT + locationOfFat / BLOCK_SIZE;
	int startWordNumber = locationOfFat % BLOCK_SIZE;
	storeValue(disk[blockNumber].word[startWordNumber + FATENTRY_FILENAME], -1);
	storeValue(disk[blockNumber].word[startWordNumber + FATENTRY_BASICBLOCK], -1);
	storeValue(disk[blockNumber].word[startWordNumber + FATENTRY_FILESIZE], 0);
	return 0;
}


/*
  This function deletes an executable file from the disk.
  NOTE: 1. Memory copy is committed to disk.
	2. Due to a technical glitch any string which is already stored on the disk will have to be searched in the
	  memory copy after appending a newline.
  
*/
int deleteExecutableFromDisk(char *name)
{
	int locationOfFat,i,blockAddresses[SIZE_EXEFILE_BASIC];   //0-basic block , 1,2,3-code+data blocks
	for(i=0;i<SIZE_EXEFILE_BASIC;i++)
		blockAddresses[i]=0;
	locationOfFat = CheckRepeatedName(name);
	if(locationOfFat >= FAT_SIZE){
		printf("File \'%s\' not found!\n",name);
		return -1;
	}
	if(strstr(name,".xsm") == NULL)
	{
		printf("\'%s\' is not a valid executable file!\n",name);
		return -1;
	}
	
	getDataBlocks(blockAddresses,locationOfFat);		
	FreeUnusedBlock(blockAddresses, SIZE_EXEFILE_BASIC);
	removeFatEntry(locationOfFat);
	for(i = FAT ; i < FAT + NO_OF_FAT_BLOCKS ; i++){
		writeToDisk(i,i);
	}
	for( i=DISK_FREE_LIST ; i<DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)
		writeToDisk(i,i);
	
	return 0;	
}

/*
  This function deletes a data file from the disk.
*/
int deleteDataFromDisk(char *name)
{
	int locationOfFat,i,blockAddresses[MAX_DATAFILE_SIZE_BASIC+1];
	for(i=0;i<MAX_DATAFILE_SIZE_BASIC;i++)
		blockAddresses[i]=0;   
	locationOfFat = CheckRepeatedName(name);
	if(locationOfFat >= FAT_SIZE)
	{
		printf("File \'%s\' not found!\n",name);
		return -1;
	}
	if(strstr(name,".dat") == NULL)
	{
		printf("\'%s\' is not a valid data file!\n",name);
		return -1;
	}
	
	getDataBlocks(blockAddresses,locationOfFat);		
	FreeUnusedBlock(blockAddresses, MAX_DATAFILE_SIZE_BASIC);
	removeFatEntry(locationOfFat);
	for(i = FAT ; i < FAT + NO_OF_FAT_BLOCKS ; i++){
		writeToDisk(i,i);
	}
	for( i=DISK_FREE_LIST ; i<DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)
		writeToDisk(i,i);
	return 0;
}



/*
  This function deletes the INIT code from the disk.
*/
int deleteINITFromDisk()
{
	emptyBlock(TEMP_BLOCK);
	int i;
	for (i=0; i<NO_OF_INIT_BLOCKS; i++)
		writeToDisk(TEMP_BLOCK,INIT_BASIC_BLOCK+i);	

	//writeToDisk(TEMP_BLOCK,INIT_BASIC_BLOCK);
	//writeToDisk(TEMP_BLOCK,INIT_BASIC_BLOCK+1);
	//writeToDisk(TEMP_BLOCK,INIT_BASIC_BLOCK+2);
	return 0;
}

/*
  This function deletes the OS code from the disk.
*/
int deleteOSCodeFromDisk()
{
	emptyBlock(TEMP_BLOCK);
	int i;
	for (i=0;i<OS_STARTUP_CODE_SIZE; i++)
		writeToDisk(TEMP_BLOCK,OS_STARTUP_CODE+i);
	return 0;
}

/*
  This function deletes the Timer Interrupt from the disk.
*/
int deleteTimerFromDisk()
{
	emptyBlock(TEMP_BLOCK);
	int i;
	for (i=0;i<TIMERINT_SIZE; i++)
		writeToDisk(TEMP_BLOCK,TIMERINT+i);
	return 0;
}

/*
  This function deletes the Interrupt <intNo> from the disk.
*/
int deleteIntCode(int intNo)
{
	emptyBlock(TEMP_BLOCK);
	int i;
	for (i=0;i<INT1_SIZE; i++)
		writeToDisk(TEMP_BLOCK,((intNo - 1) * INT1_SIZE)  + INT1 + i);
	return 0;
}

/*
  This function deletes the Exception Handler from the disk.
*/
int deleteExHandlerFromDisk()
{
	emptyBlock(TEMP_BLOCK);
	int i;
	for (i=0;i<EX_HANDLER_SIZE; i++)
		writeToDisk(TEMP_BLOCK,EX_HANDLER + i);
	return 0;
}

/*
  This function returns the address of a free block on the disk.
  The value returned will be the relative word address of the corresponding entry in the free list.
*/
int FindFreeBlock(){
	int i,j;
	for(i = DISK_FREE_LIST ; i < DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS ;i++){
		for(j = 0 ; j < BLOCK_SIZE; j++){
			if( getValue(disk[i].word[j]) == 0 ){
				storeValue( disk[i].word[j] , 1 );	
				return ((i-DISK_FREE_LIST)*BLOCK_SIZE + j);
			}
		}
	}
	return -1;	
}




/*
  This function returns an  empty fat entry if present.
  NOTE: The return address will be the relative word address corresponding to the filename entry in the basic block.
*/
int FindEmptyFatEntry(){
	int i,j,entryFound = 0,entryNumber = 0;
	for(j = FAT ; j < FAT + NO_OF_FAT_BLOCKS ; j++){
		for(i = FATENTRY_BASICBLOCK; i < BLOCK_SIZE ; i = i + FATENTRY_SIZE){
			if( getValue(disk[j].word[i]) == -1  ){
				entryNumber = (((j - FAT) * BLOCK_SIZE) + i);
				entryFound = 1;
				break;
			}
		}
		if(entryFound == 1)
			break;
	}
	if( entryNumber > FAT_SIZE ){
		printf("FAT  is full.\n");
		return -1;
	}
	return (entryNumber-FATENTRY_BASICBLOCK);
}



/*
  This function adds the name, size and basic block address of the file to corresponding entry in the fat.
  The first arguement is a relative address
*/
void AddEntryToMemFat(int startIndexInFat, char *nameOfFile, int size_of_file, int addrOfBasicBlock){
	strcpy(disk[FAT + (startIndexInFat / BLOCK_SIZE)].word[startIndexInFat % BLOCK_SIZE],nameOfFile);
	storeValue( disk[FAT + (startIndexInFat / BLOCK_SIZE)].word[startIndexInFat % BLOCK_SIZE + FATENTRY_FILESIZE] , size_of_file );
	storeValue( disk[FAT + (startIndexInFat / BLOCK_SIZE)].word[startIndexInFat % BLOCK_SIZE + FATENTRY_BASICBLOCK] , addrOfBasicBlock );
}



/*
  This file copies the necessary contents of a file to the corresponding location specified by the second arguemnt on the disk.
  The file is first copied to the memory copy of the disk. This is then committed to the actual disk file.
  NOTE: 1. EOF is set only after reading beyond the end of the file. This is the reason why the if condition is needed is needed.
	2. Also the function must read till EOF or BLOCK_SIZE line so that successive read proceeds accordingly
*/
int writeFileToDisk(FILE *f, int blockNum, int type)
{
	int i, line=0,j;
	char buffer[32],s[16],temp[100],c;
	emptyBlock(TEMP_BLOCK);
	if(type==0)			//writing files with assembly code
	{
		char *instr, *arg1, *arg2, *string_start;
		int line_count=0,flag=0,k=0;
		for(i = 0; i < (BLOCK_SIZE/2); i++)
		{
			fgets(temp,100,f);
			
			string_start=strchr(temp,'"');
			if(string_start==NULL)
			{
				for(k=0;k<31;k++)
					buffer[k]=temp[k];
				buffer[k]='\0';
			}
			else
			{
				if(strlen(string_start)<=16)
				{
					for(k=0;k<31;k++)
						buffer[k]=temp[k];
					buffer[k]='\0';
				}
				else
				{
					for(k=0;k<(strlen(temp)-strlen(string_start)+15);k++)
					{
						buffer[k]=temp[k];
					}
					buffer[k-1]='"';
					buffer[k]='\0';
				}
			}
		
			
			if(strlen(buffer)>3)
			{
				if(buffer[strlen(buffer)-1]=='\n')
					buffer[strlen(buffer)-1]='\0';
				instr=strtok(buffer," ");
				arg1=strtok(NULL," ");
				arg2=strtok(NULL,",");
			
				bzero(s,16);
				if(arg1!=NULL)
				{
					sprintf(s,"%s %s",instr,arg1);
					for(j=strlen(s);j<16;j++)
						s[j]='\0';
					strcpy(disk[TEMP_BLOCK].word[line_count],s);
					if(arg2!=NULL)
					{
						strcpy(s,arg2);
						for(j=strlen(s);j<16;j++)
							s[j]='\0';
						strcpy(disk[TEMP_BLOCK].word[line_count+1],s);
				
					}
					else
					{
						for(j=0;j<16;j++)
							s[j]='\0';
						strcpy(disk[TEMP_BLOCK].word[line_count+1],s);
					}
					line_count=line_count+2;
				}
				else
				{
					sprintf(s,"%s",instr);
					for(j=strlen(s);j<=16;j++)
						strcat(s,"\0");
					strcpy(disk[TEMP_BLOCK].word[line_count],s);
					bzero(s,16);
					for(j=0;j<16;j++)
						s[j]='\0';
					strcpy(disk[TEMP_BLOCK].word[line_count+1],s);
					line_count=line_count+2;
			
				}
			
			}
			
			 if(feof(f)){
				strcpy(disk[TEMP_BLOCK].word[line_count], "");
				writeToDisk(TEMP_BLOCK,blockNum);
				return -1;
			 }
			
		}
		writeToDisk(TEMP_BLOCK,blockNum);
		return 1;
	}	
	else if(type==1)			//writing data files
	{
		char buffer1[16],c;
		for(i = 0; i < BLOCK_SIZE; i++)
		{
			fgets(buffer1,16,f);
			strcpy(disk[TEMP_BLOCK].word[i],buffer1);
			if(feof(f))
			{
				strcpy(disk[TEMP_BLOCK].word[i], "");
				writeToDisk(TEMP_BLOCK,blockNum);
				return -1;
			}	
		}
		writeToDisk(TEMP_BLOCK,blockNum);
		return 1;
	}

}




/*
  This function loads the executable file corresponding to the first arguement to an appropriate location on the disk.
  This function systematically uses the above functions to do this action.
*/
int loadExecutableToDisk(char *name)
{
	FILE *fileToBeLoaded;
	int freeBlock[SIZE_EXEFILE_BASIC];
	int i,j,k,l,file_size=0,num_of_lines=0,num_of_blocks_reqd=0;
	for(i=0;i<SIZE_EXEFILE_BASIC;i++)
		freeBlock[i]=-1;
	char c='\0',*s;
	char filename[50];
	s = strrchr(name,'/');
	if(s!=NULL)
		strcpy(filename,s+1);
	else
		strcpy(filename,name);	
	
	filename[15]='\0';
		
	addext(filename,".xsm");

	expandpath(name);
	fileToBeLoaded = fopen(name, "r");
	if(fileToBeLoaded == NULL){
	    printf("File %s not found.\n", name);
	    return -1;
	  }
	if(fileToBeLoaded == NULL){
		printf("The file could not be opened");
		return -1;
	}
	
	while(c!=EOF)
	{
		c=fgetc(fileToBeLoaded);
		if(c=='\n')
			num_of_lines++;
	}
	
	num_of_blocks_reqd = (num_of_lines / (BLOCK_SIZE/2)) + 1;
	
	if(num_of_blocks_reqd > SIZE_EXEFILE)
	{
		printf("The size of file exceeds %d blocks",SIZE_EXEFILE);
		return -1;
	}
	
	fseek(fileToBeLoaded,0,SEEK_SET);
	
	for(i = 0; i < num_of_blocks_reqd + 1; i++)
	{
		if((freeBlock[i] = FindFreeBlock()) == -1){
				printf("Insufficient disk space!\n");
				FreeUnusedBlock(freeBlock, SIZE_EXEFILE_BASIC);
				return -1;
			}
	}
	i = CheckRepeatedName(filename);
	if( i < FAT_SIZE ){
		printf("Disk already contains the file with this name. Try again with a different name.\n");
		FreeUnusedBlock(freeBlock, SIZE_EXEFILE_BASIC);
		return -1;
	}
	
	k = FindEmptyFatEntry();		
	if( k == -1 ){
		FreeUnusedBlock(freeBlock, SIZE_EXEFILE_BASIC);
		printf("No free FAT entry found.\n");
		return -1;			
	}
	
	
	for(i = DISK_FREE_LIST ;i < DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)		//updating disk free list in disk
		writeToDisk(i, i);
	emptyBlock(TEMP_BLOCK);				//note:need to modify this
	
	for( i = 1 ; i < SIZE_EXEFILE_BASIC ; i++ )
	{
		storeValue(disk[TEMP_BLOCK].word[i-1],freeBlock[i]); 
	}
	writeToDisk(TEMP_BLOCK,freeBlock[0]);
	
	for(i=0;i<num_of_blocks_reqd;i++)
	{
		j = writeFileToDisk(fileToBeLoaded, freeBlock[i+1], ASSEMBLY_CODE);
		file_size++;
	}
	


	AddEntryToMemFat(k, filename, file_size * BLOCK_SIZE, freeBlock[0]);	
	for(i = FAT; i < FAT + NO_OF_FAT_BLOCKS ; i++){
		writeToDisk(i,i);				
	}
	
      close(fileToBeLoaded);
      return 0;
}



/*
  This function loads a data file to the disk.
*/
int loadDataToDisk(char *name)
{
	FILE *fileToBeLoaded;
	int freeBlock[MAX_DATAFILE_SIZE_BASIC];
	int i,j,k,num_of_chars=0,num_of_blocks_reqd=0,file_size=0,num_of_words=0;
	for(i=0;i<MAX_DATAFILE_SIZE_BASIC;i++)
		freeBlock[i]=-1;
	char c='\0',*s;
	char filename[50],buf[16];
	s = strrchr(name,'/');
	if(s!=NULL)
		strcpy(filename,s+1);
	else
		strcpy(filename,name);	
	
	filename[15]='\0';
	addext(filename,".dat");

	expandpath(name);
	fileToBeLoaded = fopen(name, "r");
	if(fileToBeLoaded == NULL)
	{
		printf("File \'%s\' not found.!\n", name);
		return -1;
	}
	if(fileToBeLoaded == NULL)
	{
		printf("The file could not be opened!");
		return -1;
	}
	
	fseek(fileToBeLoaded, 0L, SEEK_END);
	
	num_of_chars = ftell(fileToBeLoaded);
	
	fseek(fileToBeLoaded,0,SEEK_SET);
	while(1)
	{
		fgets(buf,16,fileToBeLoaded);
		num_of_words++;
		if(feof(fileToBeLoaded))
			break;
	}
	num_of_blocks_reqd = (num_of_words/512) + 1;
	//printf("\n Chars = %d, Words = %d, Blocks(chars) = %d, Blocks(words) = %d",num_of_chars,num_of_words,num_of_blocks_reqd,(num_of_words/512));
	if(num_of_blocks_reqd > MAX_DATAFILE_SIZE)
	{
		printf("The size of file exceeds %d blocks",MAX_DATAFILE_SIZE);
		return -1;
	}
	
	fseek(fileToBeLoaded,0,SEEK_SET);
	
	for(i = 0; i < num_of_blocks_reqd + 1; i++)
	{
		if((freeBlock[i] = FindFreeBlock()) == -1){
				printf("not sufficient space in disk to hold a new file.\n");
				FreeUnusedBlock(freeBlock, MAX_DATAFILE_SIZE_BASIC);
				return -1;
			}
	}
	i = CheckRepeatedName(filename);
	if( i < FAT_SIZE )
	{
		printf("Disk already contains the file with this name. Try again with a different name.\n");
		FreeUnusedBlock(freeBlock, MAX_DATAFILE_SIZE_BASIC);
		return -1;
	}
	
	k = FindEmptyFatEntry();		
	if( k == -1 )
	{
		FreeUnusedBlock(freeBlock, MAX_DATAFILE_SIZE_BASIC);
		printf("No free FAT entry found.\n");
		return -1;			
	}
	
	
	for(i = DISK_FREE_LIST ;i < DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)		//updating disk free list in disk
		writeToDisk(i, i);
	emptyBlock(TEMP_BLOCK);				//note:need to modify this
	
	for( i = 1 ; i < MAX_DATAFILE_SIZE_BASIC ; i++ )
	{
		storeValue(disk[TEMP_BLOCK].word[i-1],freeBlock[i]); 
	}
	writeToDisk(TEMP_BLOCK,freeBlock[0]);
	
	for(i=0;i<num_of_blocks_reqd;i++)
	{
		j = writeFileToDisk(fileToBeLoaded, freeBlock[i+1], DATA_FILE);
		file_size++;
	}
	
	  
	AddEntryToMemFat(k, filename, file_size * BLOCK_SIZE, freeBlock[0]);		
	for(i = FAT; i < FAT + NO_OF_FAT_BLOCKS ; i++){
		writeToDisk(i,i);				//updating disk fat entry note:check for correctness
	}
	
      close(fileToBeLoaded);
      return 0;
	
}


/*
  This function copies the init program to its proper location on the disk.
*/
int loadINITCode(char* fileName )
{
	FILE * fp;
	int i,j;
	expandpath(fileName);
	fp = fopen(fileName, "r");
	if(fp == NULL)
	{
		printf("File \'%s\' not found.\n", fileName);
		return -1;
	}
	
	for(i=0; i<NO_OF_INIT_BLOCKS; i++)
	{
		j = writeFileToDisk(fp, INIT_BASIC_BLOCK + i, ASSEMBLY_CODE);
		if(j != 1)
			break;
	}
	//j = writeFileToDisk(fp, INIT_BASIC_BLOCK, ASSEMBLY_CODE);		//writing executable file to disk
	
	//if(j == 1)
	//	j = writeFileToDisk(fp, INIT_BASIC_BLOCK + 1, ASSEMBLY_CODE);		//if the file is longer than one page.  
	//if(j == 1)
		//writeFileToDisk(fp, INIT_BASIC_BLOCK + 2, ASSEMBLY_CODE);
	close(fp);
	return 0;
  
}



/*
  This function loads the OS startup code specified by the first arguement to its appropriate location on disk.
  The code is first copied to memory copy. If this copying proceeds properly then the memory copy is committed to the disk.
*/
int loadOSCode(char* fileName){

	emptyBlock(TEMP_BLOCK);
	writeToDisk(TEMP_BLOCK,OS_STARTUP_CODE);
	expandpath(fileName);
	FILE* fp = fopen(fileName, "r");
	int i,j;
	if(fp == NULL)
	{
		printf("File \'%s\' not found.\n", fileName);
		return -1;
	}
	
	for(i=0;i<OS_STARTUP_CODE_SIZE;i++)
	{
		j = writeFileToDisk(fp, OS_STARTUP_CODE + i, ASSEMBLY_CODE);
		if (j != 1)
			break;
	}
	if(j==1)
	{
		printf("OS Code exceeds %d block\n",OS_STARTUP_CODE_SIZE);
		deleteOSCodeFromDisk();
		//emptyBlock(TEMP_BLOCK);
		//writeToDisk(TEMP_BLOCK,OS_STARTUP_CODE);
	}
	close(fp);
	return 0;
}


/*
  This function copies the interrupts to the proper location on the disk.
*/
int loadIntCode(char* fileName, int intNo)
{
	expandpath(fileName);
	FILE* fp = fopen(fileName, "r");
	int i,j;
	if(fp == NULL)
	{
		printf("File \'%s\' not found.\n", fileName);
		return -1;
	}
	
	for(i=0;i<INT1_SIZE;i++)
	{
		//printf ("Int block 1 - %d = %d\n",i,((intNo - 1) * INT1_SIZE)  + INT1 + i);
		j = writeFileToDisk(fp, ((intNo - 1) * INT1_SIZE)  + INT1 + i, ASSEMBLY_CODE);
		if(j != 1)
			break;
	}
	if(j==1)
	{
		printf("Interrupt Code exceeds %d block\n",INT1_SIZE);
		deleteIntCode(intNo);
		//emptyBlock(TEMP_BLOCK);
		//writeToDisk(TEMP_BLOCK,intNo + INT1 -1);
	}
	close(fp);
	return 0;
}

/*
  This function copies the timer interrupt to the proper location on the disk.
*/
int loadTimerCode(char* fileName)
{
	expandpath(fileName);
	FILE* fp = fopen(fileName, "r");
	int i,j;
	if(fp == NULL)
	{
		printf("File %s not found.\n", fileName);
		return -1;
	}
	
	for(i=0;i<TIMERINT_SIZE;i++)
	{
		j = writeFileToDisk(fp, TIMERINT + i, ASSEMBLY_CODE);
		if (j != 1)
			break;
	}
	if(j==1)
	{
		printf("Timer Interrupt Code exceeds %d block\n",TIMERINT_SIZE);
		deleteTimerFromDisk();
		//emptyBlock(TEMP_BLOCK);
		//writeToDisk(TEMP_BLOCK,TIMERINT);
	}
	close(fp);
	return 0;
}

/*
  This function copies the exception handler to the proper location on the disk.
*/
int loadExHandlerToDisk(char* fileName)
{
	expandpath(fileName);
	FILE* fp = fopen(fileName, "r");
	int i,j;
	if(fp == NULL)
	{
		printf("File %s not found.\n", fileName);
		return -1;
	}
	
	for(i=0;i<EX_HANDLER_SIZE;i++)
	{
		j = writeFileToDisk(fp, EX_HANDLER + i, ASSEMBLY_CODE);
		if(j != 1)
			break;
	}
	if(j==1)
	{
		printf("Exception Handler exceeds one block\n");
		deleteExHandlerFromDisk();
		//emptyBlock(TEMP_BLOCK);
		//writeToDisk(TEMP_BLOCK,EX_HANDLER);
	}
	close(fp);
	return 0;
}



/*
  This function displays the content of the files stored in the disk.
*/
void displayFileContents(char *name)
{
	int fd;
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd < 0){
	  printf("Unable to Open Disk File!\n");
	  return;
	}
	
	close(fd);
	int i,j,k,l,flag=0,locationOfFat;
	int blk[512];
	
	for(i=0;i<511;i++)
		blk[i] = 0;
	
	locationOfFat = CheckRepeatedName(name);
	if(locationOfFat >= FAT_SIZE){
		printf("File \'%s\' not found!\n",name);
		return;
	}
	
	
	getDataBlocks(blk,locationOfFat);

	k = 1;
	while (blk[k] > 0)
	{
		emptyBlock(TEMP_BLOCK);
		readFromDisk(TEMP_BLOCK,blk[k]);
		for(l=0;l<BLOCK_SIZE;l++)
		{
			if(strcmp(disk[TEMP_BLOCK].word[l],"\0")!=0)
				printf("%s   \n",disk[TEMP_BLOCK].word[l]);
		}
		//printf("next block\n");
		emptyBlock(TEMP_BLOCK);
		k++;
	}
}

/*
  This function copies the contents of the disk starting from <startBlock> to <endBlock> to a unix file.
*/
void copyBlocksToFile (int startblock,int endblock,char *filename)
{
	int fd;
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd < 0){
	  printf("Unable to Open Disk File!\n");
	  return;
	}
	close(fd);
	int i,j;
	FILE *fp;
	expandpath(filename);
	fp = fopen(filename,"w");
	if(fp == NULL)
	{
		printf("File \'%s\' not found!\n", filename);
	}
	else
	{
		for(i = startblock; i <= endblock; i++)
		{
			emptyBlock(TEMP_BLOCK);
			readFromDisk(TEMP_BLOCK,i);
			for(j=0;j<BLOCK_SIZE;j++)
			{
				fprintf(fp,"%s\n",disk[TEMP_BLOCK].word[j]);
			}
		}
		fclose(fp);
	}
	
}

/*
  This function displays disk free list and the amount of free space in the disk.
*/
void displayDiskFreeList()
{
	int fd;
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd < 0){
	  printf("Unable to Open Disk File!\n");
	  return;
	}
	close(fd);
	int i,j,no_of_free_blocks=0;
	for(j = 0; j < NO_OF_FREE_LIST_BLOCKS; j++)
	{
		for(i = 0; i < BLOCK_SIZE; i++)
		{
			printf("%d \t - \t %s  \n",i,disk[DISK_FREE_LIST+j].word[i]);
			if(getValue(disk[DISK_FREE_LIST+j].word[i])==0)
				no_of_free_blocks++;
		}
	}
	printf("\nNo of Free Blocks = %d",no_of_free_blocks);
	printf("\nTotal no of Blocks = %d",NO_OF_DISK_BLOCKS);
}



// To expand environment variables in path
void expandpath(char *path) 		
{
	char *rem_path = strdup(path);
	char *token = strsep(&rem_path, "/");
	if(rem_path!=NULL)
		sprintf(path,"%s/%s",getenv(++token)!=NULL?getenv(token):token-1,rem_path);
	else
		sprintf(path,"%s",getenv(++token)!=NULL?getenv(token):token-1);
}

void addext(char *filename, char *ext)
{
	int l = strlen(filename);
	if(l>=16)
	{
		strcpy(filename+11,ext);
		return;
	}
	if(strcmp(filename+l-4,ext)!=0)
	{
		strcat(filename,ext);
		l = strlen(filename);

		if(l>=16)
		{
			strcpy(filename+11,ext);
			return;
		}
	}
}
