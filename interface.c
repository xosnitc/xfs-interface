#include <string.h>
#include "interface.h"


// Command Line interface
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
  	runCommand(command);
  }

}

/* Function to process commands */

void runCommand(char command[])
{
	char *name = strtok(command, " ");
	char *arg1, *arg2, *arg3;
	
	if(strcmp(name,"help")==0)
	{
		printf(" fdisk \n\t Format the disk with XFS filesystem\n\n");	
		printf(" load --exec  <unix_filename> <xfs_filename> \n\t Loads an executable file to XFS disk \n\n");
		printf(" load --data	 <unix_filename>  <xfs_filename>\n\t Loads a data file to XFS disk \n\n");
		printf(" load --os  <unix_filename> \n\t Loads OS startup code to XFS disk \n\n");
		printf(" load --int=timer <unix_filename>\n\t Loads Timer Interrupt routine to XFS disk \n\n");		
		printf(" load --int=[1-7] <unix_filename>\n\t Loads the specified Interrupt routine to XFS disk \n\n");
		printf(" load --exhandler <unix_filename>  \n\t Loads exception handler routine to XFS disk \n\n");
		printf(" remove --exec <xfs_filename>\n\t Removes an executable file from XFS disk \n\n");
		printf(" remove --data	<xfs_filename>\n\t Removes a data file from XFS disk \n\n");
		printf(" remove --os \n\t Removes OS startup code from XFS disk \n\n");
		printf(" remove --int=timer \n\t Removes the Timer Interrupt routine from XFS disk \n\n");	
		printf(" remove --int=[1-7] \n\t Removes the specified Interrupt routine from XFS disk \n\n");
		printf(" rempve--exhandler\n\t Removes the exception handler routine from XFS disk \n\n");
		printf(" ls \n\t List all files\n\n");	
		printf(" df \n\t Display free list and free space\n\n");
		printf(" cat <xfs_filename> \n\t to display contents of a file\n\n");
		printf(" copy <start_blocks> <end_block> <unix_filename>\n\t Copies contents of specified range of blocks to a UNIX file.\n");	
	}
	
	
	else if (strcmp(name,"fdisk")==0)
	{
		printf("Formatting Complete. \"disk.xfs\" created.\n");
		createDisk(FORMAT);		
	}

	else if (strcmp(name,"load")==0)
	{
		arg1 = strtok(NULL, " ");
		arg2 = strtok(NULL, " ");
		arg3 = strtok(NULL, " ");
		printf("%s",arg3);
	}
		
	
	else if (strcmp(name,"ls")==0)
	{
		listAllFiles();
	}
   // token = strtok(NULL, " ");
	
}
/*	
	


      case '2':
	printf("\nEnter Filename\n");
	scanf("%s", fileName);
	fileName[WORD_SIZE - 1] = '\0';
	loadExecutableToDisk(fileName);
	break;
      case '3':
	printf("\nEnter Filename\n");
	scanf("%s", fileName);
	fileName[WORD_SIZE - 1] = '\0';
	deleteExecutableFromDisk(fileName);
	break;
      case '4':
	printf("\nEnter Filename\n");
	scanf("%s", fileName);
	fileName[WORD_SIZE - 1] = '\0';
	loadOSCode(fileName);
	break;
      case '5':
	printf("\nEnter interrupt no:\n");
	scanf("%d", &intNo);
	printf("Enter filename\n");
	scanf("%s", fileName);
	loadIntCode(fileName, intNo);
	break;
      case '6':
	
	break;
      case '7':
	printf("\nEnter Filename\n");
	scanf("%s", fileName);
	fileName[WORD_SIZE - 1] = '\0';
	loadINITCode(fileName);
	break;
      case '8':
	printf("Exiting...........\n");
	exit(0);
	break;
      case '9':
	loadOSCode("startup.esim");
	loadIntCode("int0.esim", 0);
	loadIntCode("int1.esim", 1);
	loadIntCode("int2.esim", 2);
	loadIntCode("int3.esim", 3);
	loadIntCode("int4.esim", 4);
	loadIntCode("int5.esim", 5);
	loadIntCode("int6.esim", 6);
	loadIntCode("int7.esim", 7);
	loadINITCode("init.esim");
	break;
      default:
	printf("Invalid option\n");
	break;
    }
    scanf("%c", &option);
    while(option != '\n')
      scanf("%c", &option);
  }
*/

/*  printf("\t1. List All Files\n");
  printf("\t2. Load Executable File\n");
  printf("\t3. Remove Executable File\n");
  printf("\t4. Load OS Start up Code\n");
  printf("\t5. Load Interrupts\n");
  printf("\t6. Format/Create disk\n");
  printf("\t7. Load INIT Code\n");
  printf("\t8. Exit\n");
  printf("\t9. Load everything :)\n");
  printf("Option No:\n"); */





int main(){
  int  intNo;
  char fileName[WORD_SIZE], option;
  FILE* diskFp;
  createDisk(DO_NOT_FORMAT); //in case the disk file is not present
  loadFileToVirtualDisk();
  cli();
  return 0;
}
