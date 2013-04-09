DISK = disk

all: xfs-interface

xfs-interface: fileSystem.h fileSystem.c fileUtility.h fileUtility.h interface.h interface.c createDisk.h createDisk.c
	gcc -g fileSystem.c fileUtility.c interface.c createDisk.c -o xfs-interface

clean:
	rm -rf $(DISK) *.o xfs-interface 2> /dev/null
