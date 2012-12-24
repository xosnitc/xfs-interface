#ifndef INTERFACE_H
#define INTERFACE_H
#include "fileSystem.h"
#include "createDisk.h"
#define DO_NOT_FORMAT 0
#define FORMAT 1

/*
  This function invokes a command line interface for interacting with XFS.
*/
void cli();	

/*
 This function processes each command the user enters
*/
void runCommand(char command[]);


#endif
