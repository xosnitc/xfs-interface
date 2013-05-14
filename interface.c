#include <string.h>
#include <libgen.h>
#include "interface.h"


/* 
Function to invoke Command Line interface 
*/
void cli(int argc, char **argv)
{
	char command[100], c;
	int i,j;
	if(argc>1)
	{
		strcpy(command,argv[1]);
		i=2;
		while(i<argc)
		{
		
			sprintf(command,"%s %s", command, argv[i]);
			i++;
		}		
		runCommand(command);	
	}
	else
	{
		printf("Unix-XFS Interace Version 1.0. \nType \"help\" for  getting a list of commands.\n");
		while(1)
		{
			i=0;
			printf("# ");
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
		printf(" fdisk \n\t Format the disk with XFS filesystem\n");
		printf(" load --exec  <pathname>  \n\t Loads an executable file to XFS disk \n");
		printf(" load --init  <pathname> \n\t Loads INIT code to XFS disk \n");
		printf(" load --data <pathname> \n\t Loads a data file to XFS disk \n");
		printf(" load --os  <pathname> \n\t Loads OS startup code to XFS disk \n");
		printf(" load --int=timer <pathname>\n\t Loads Timer Interrupt routine to XFS disk \n");
		printf(" load --int=[1-7] <pathname>\n\t Loads the specified Interrupt routine to XFS disk \n");
		printf(" load --exhandler <pathname>  \n\t Loads exception handler routine to XFS disk \n");
		printf(" rm --exec <xfs_filename>\n\t Removes an executable file from XFS disk \n");
		printf(" rm --init <xfs_filename> \n\t Removes INIT code from XFS disk \n");
		printf(" rm --data <xfs_filename>\n\t Removes a data file from XFS disk \n");
		printf(" rm --os \n\t Removes OS startup code from XFS disk \n");
		printf(" rm --int=timer \n\t Removes the Timer Interrupt routine from XFS disk \n");
		printf(" rm --int=[1-7] \n\t Removes the specified Interrupt routine from XFS disk \n");
		printf(" rm --exhandler\n\t Removes the exception handler routine from XFS disk \n");
		printf(" ls \n\t List all files\n");
		printf(" df \n\t Display free list and free space\n");
		printf(" cat <xfs_filename> \n\t to display contents of a file\n");
		printf(" copy <start_blocks> <end_block> <unix_filename>\n\t Copies contents of specified range of blocks to a UNIX file.\n");
		printf(" exit \n\t Exit the interface\n");
	}
	
	
	else if (strcmp(name,"fdisk")==0)	//formatting the disk with XFS partition
	{
		printf("Formatting Complete. \"disk.xfs\" created.\n");
		createDisk(FORMAT);		
	}
	
	else if (strcmp(name,"load")==0) 	//loads files to XFS disk.
	{
		int fd;
		fd = open(DISK_NAME, O_RDONLY, 0666);
		if(fd < 0){
		  printf("Unable to Open Disk File\n");
		  return;
		}
		close(fd);
		arg1 = strtok(NULL, " ");
		arg2 = strtok(NULL, " ");	

		char *int_command = strtok(arg1, "=");	
		char *intType = strtok(NULL, "=");
	    	char *fileName = arg2;;
	    
		if(fileName!=NULL)
			fileName[50] = '\0';
		else
		{
			printf("Missing <pathname> for load. See \"help\" for more information\n");
			return;
		}				
		if (strcmp(arg1,"--exec")==0)	
		{
			char *c;
			if (strlen(basename(fileName)) > 12)
			{
				printf("Filename is more than 12 characters long\n");
				return;
			}
			
			c = strrchr(fileName,'.');
			if (c == NULL || strcmp(c,".xsm") != 0)
			{
				printf("Filename does not have \".xsm\" extension\n");
				return;
			}
			
			loadExecutableToDisk(fileName);	 //loads executable file to disk.
		}	
			
		else if (strcmp(arg1,"--init")==0)	
			loadINITCode(fileName);			 //loads init code to disk
		else if (strcmp(arg1,"--data")==0) 
		{
			char *c;
			if (strlen(basename(fileName)) > 12)
			{
				printf("Filename is more than 12 characters long\n");
				return;
			}
			
			c = strrchr(fileName,'.');
			if (c == NULL || strcmp(c,".dat") != 0)
			{
				printf("Filename does not have \".dat\" extension\n");
				return;
			}
			
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
				if(intNo >=1 && intNo <=NO_OF_INTERRUPTS)
					loadIntCode(fileName, intNo);
				else
				{
					printf("Invalid argument for \"--int=\" \n");
					return;
				}
			}
		}
		else if (strcmp(arg1,"--exhandler")==0) 
			{
				loadExHandlerToDisk(fileName);		 //loads exception handler routine to disk.
			}
		else
			printf("Invalid argument \"%s\" for load. See \"help\" for more information\n",arg1);
	}	
	
	else if (strcmp(name,"rm")==0) 	//removes files to XFS disk.
	{
		int fd;
		fd = open(DISK_NAME, O_RDONLY, 0666);
		if(fd < 0){
		  printf("Unable to Open Disk File\n");
		  return;
		}
		close(fd);
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
				printf("Missing <xfs_filename> for rm. See \"help\" for more information\n");
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
				printf("Missing <xfs_filename> for rm. See \"help\" for more information\n");
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
				if(intNo >=1 && intNo <= NO_OF_INTERRUPTS)
					deleteIntCode(intNo);				//removes Int Code from disk.
				else
				{
					printf("Invalid argument for \"--int=\" \n");
					return;
				}
			}
		}
		else if (strcmp(arg1,"--exhandler")==0)
			{
				deleteExHandlerFromDisk();			 //removes exception handler routine from disk.			
			}
		else
			printf("Invalid argument \"%s\" for rm. See \"help\" for more information\n",arg1);
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
			printf("Missing <xfs_filename> for cat. See \"help\" for more information\n");
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
			printf("Insufficient arguments for \"copy\". See \"help\" for more information\n");
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
		printf("Unknown command \"%s\". See \"help\" for more information\n",name);
}



int main(int argc, char **argv){
	int  intNo, fd;
	char fileName[51], option;
	FILE* diskFp;
	
	fd = open(DISK_NAME, O_RDONLY, 0666);
	if(fd > 0)
	{
		loadFileToVirtualDisk();
	}
	close(fd);
		
	cli(argc, argv);					//Loads the Command Line Interface
	return 0;
}
