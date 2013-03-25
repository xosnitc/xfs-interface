#include <string.h>
#include "interface.h"


/* 
Function to invoke Command Line interface 
*/
void cli()
{
  char command[100], c;
  int i,j;
		
  printf("Unix-XFS Interace Version 1.0. \nType \"help\" for  getting a list of commands.");
  while(1)
  {
  	i=0;
  	printf("\n# ");
  	scanf("%c",&c);
  	while(c!='\n')
  	{  	
		command[i++] = c;
		scanf("%c",&c);
  	}
  	command[i] = '\0';
	if(command[0]!='\0')
  			runCommand(command);
  }
}

/* 
Function to process commands 
*/
void runCommand(char command[])
{
	char *name = strtok(command, " ");
	char *arg1, *arg2, *arg3;
	
	
	if(strcmp(name,"help")==0)		//"help" to display all commands
	{
		printf("\n fdisk \n\t Format the disk with XFS filesystem\n\n");	
		printf(" load --exec  <pathname>  \n\t Loads an executable file to XFS disk \n\n");
		printf(" load --init  <pathname> \n\t Loads INIT code to XFS disk \n\n");
		printf(" load --data <pathname> \n\t Loads a data file to XFS disk \n\n");
		printf(" load --os  <pathname> \n\t Loads OS startup code to XFS disk \n\n");
		printf(" load --int=timer <pathname>\n\t Loads Timer Interrupt routine to XFS disk \n\n");		
		printf(" load --int=[1-7] <pathname>\n\t Loads the specified Interrupt routine to XFS disk \n\n");
		printf(" load --exhandler <pathname>  \n\t Loads exception handler routine to XFS disk \n\n");
		printf(" rm --exec <xfs_filename>\n\t Removes an executable file from XFS disk \n\n");
		printf(" rm --init <xfs_filename> \n\t Removes INIT code from XFS disk \n\n");	
		printf(" rm --data <xfs_filename>\n\t Removes a data file from XFS disk \n\n");
		printf(" rm --os \n\t Removes OS startup code from XFS disk \n\n");
		printf(" rm --int=timer \n\t Removes the Timer Interrupt routine from XFS disk \n\n");	
		printf(" rm --int=[1-7] \n\t Removes the specified Interrupt routine from XFS disk \n\n");
		printf(" rm --exhandler\n\t Removes the exception handler routine from XFS disk \n\n");
		printf(" ls \n\t List all files\n\n");	
		printf(" df \n\t Display free list and free space\n\n");
		printf(" cat <xfs_filename> \n\t to display contents of a file\n\n");
		printf(" copy <start_blocks> <end_block> <unix_filename>\n\t Copies contents of specified range of blocks to a UNIX file.\n\n");	
		printf(" exit \n\t Exit the interface\n");
	}
	
	
	else if (strcmp(name,"fdisk")==0)	//formatting the disk with XFS partition
	{
		printf("Formatting Complete. \"disk.xfs\" created.\n");
		createDisk(FORMAT);		
	}
	
	else if (strcmp(name,"load")==0) 	//loads files to XFS disk.
	{
		arg1 = strtok(NULL, " ");
		arg2 = strtok(NULL, " ");	

		char *int_command = strtok(arg1, "=");	
		char *intType = strtok(NULL, "=");
	    	char *fileName = arg2;;
	    
		if(fileName!=NULL)
			fileName[50] = '\0';
		else
		{
			printf("Missing <pathname> for load. See \"help\" for more information");
			return;
		}				
		if (strcmp(arg1,"--exec")==0)		
			loadExecutableToDisk(fileName);	 //loads executable file to disk.
		else if (strcmp(arg1,"--init")==0)	
			loadINITCode(fileName);			 //loads init code to disk
		else if (strcmp(arg1,"--data")==0) 
			{
				loadDataToDisk(fileName);		 //loads data file to disk.
			}
		else if (strcmp(arg1,"--os")==0)
			loadOSCode(fileName);			//loads OS startup code to disk
		else if (strcmp(arg1,"--int")==0)
		{
			if(strcmp(intType,"timer")==0)
			{
				loadTimerCode(fileName);
			}
			else
			{
				int intNo = atoi(intType);
				loadIntCode(fileName, intNo);
			}
		}
		else if (strcmp(arg1,"--exhandler")==0) 
			{
				loadExHandlerToDisk(fileName);		 //loads exception handler routine to disk.
			}
		else
			printf("Invalid argument \"%s\" for load. See \"help\" for more information",arg1);
	}	
	
	else if (strcmp(name,"rm")==0) 	//removes files to XFS disk.
	{
		arg1 = strtok(NULL, " ");
		arg2 = strtok(NULL, " ");	
		
		char *int_command = strtok(arg1, "=");	
		char *intType = strtok(NULL, "=");
	    	char *fileName = arg2;;
	    
		if(fileName!=NULL)
			fileName[50] = '\0';				
		if (strcmp(arg1,"--exec")==0)		
		{
			if(fileName==NULL)
			{
				printf("Missing <xfs_filename> for rm. See \"help\" for more information");
				return;
			}
			deleteExecutableFromDisk(fileName);	 	//removes executable file fron disk.
		}
		else if (strcmp(arg1,"--init")==0)	
			{
				deleteINITFromDisk();			 	//removes init code from disk
			}
		else if (strcmp(arg1,"--data")==0) 
		{
			if(fileName==NULL)
			{
				printf("Missing <xfs_filename> for rm. See \"help\" for more information");
				return;
			}
			deleteDataFromDisk(fileName);			 //removes data file from disk..		
		}
		else if (strcmp(arg1,"--os")==0)
			{
				deleteOSCodeFromDisk();			 	//removes OS code from disk.		
			}
		else if (strcmp(arg1,"--int")==0)
		{
			if(strcmp(intType,"timer")==0)
				{
					deleteTimerFromDisk();				//removes Timer interrupt routine from disk.
				}
			else
			{
				int intNo = atoi(intType);
				deleteIntCode(intNo);				//removes Int Code from disk.
			}
		}
		else if (strcmp(arg1,"--exhandler")==0)
			{
				deleteExHandlerFromDisk();			 //removes exception handler routine from disk.			
			}
		else
			printf("Invalid argument \"%s\" for rm. See \"help\" for more information",arg1);
	}	
	
	else if (strcmp(name,"ls")==0)		//Lists all files.
		listAllFiles();
		
	else if (strcmp(name,"df")==0)		//Lists disk free list
		displayDiskFreeList();
				
	else if (strcmp(name,"cat")==0)		//Displays contents of a file
	{
		arg1 = strtok(NULL, " ");
		char *fileName = arg1;;
		if(fileName!=NULL)
		{
			//fileName[WORD_SIZE+1] = '\n';
			
			displayFileContents(fileName);
		}
		else
		{
			printf("Missing <xfs_filename> for cat. See \"help\" for more information");
			return;
		}	
	}
	else if (strcmp(name,"copy")==0)		//Copies blocks from Disk to UNIX file.
	{
		arg1 = strtok(NULL, " ");
		arg2 = strtok(NULL, " ");
		arg3 = strtok(NULL, " ");
		if(arg1==NULL || arg2==NULL|| arg3==NULL)
		{
			printf("Insufficient arguments for \"copy\". See \"help\" for more information");
			return;
		}	
		else
		{
			int startBlock = atoi(arg1);
			int endBlock = atoi(arg2);	
			char *fileName = arg3;			
			fileName[50] = '\0';
			copyBlocksToFile (startBlock,endBlock,fileName);
		}	
	}						
	else if (strcmp(name,"exit")==0)		//Exits the interface
		exit(0);
	else
		printf("Unknown command \"%s\". See \"help\" for more information",name);
}



int main(){
	int  intNo, fd;
	char fileName[51], option;
	FILE* diskFp;
	
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd > 0)
	{
		loadFileToVirtualDisk();
	}
	close(fd);
		
	cli();					//Loads the Command Line Interface
	return 0;
}
