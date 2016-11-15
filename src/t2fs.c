#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/bitmap2.h"


int identify2 (char *name, int size) {
	
	char grupo[] = "Ana Mativi\nAthos Lagemann\nRicardo Sabedra\n";
	int tamanho = sizeof(grupo);

	//if (init())
	//  return fatal("identify2: Could not initialize T2FS", NOINFO, 0);

	if (size <= tamanho){
		return -1;
	}

	memcpy(name, &grupo, tamanho); // Copia a string

	return 0;
}

FILE2 create2 (char *filename) {

	return -1;
}

int delete2 (char *filename) {

	return -1;
}

FILE2 open2 (char *filename) {

	return -1;
}

int close2 (FILE2 handle) {

	return -1;
}

int read2 (FILE2 handle, char *buffer, int size) {

	return -1;
}

int write2 (FILE2 handle, char *buffer, int size) {

	return -1;
}

int truncate2 (FILE2 handle) {

	return -1;
}

int seek2 (FILE2 handle, DWORD offset) {

	return -1;
}

int mkdir2 (char *pathname) {

	return -1;
}

int rmdir2 (char *pathname) {

	return -1;
}

DIR2 opendir2 (char *pathname) {

	return -1;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry) {

	return -1;
}

int closedir2 (DIR2 handle) {

	return -1;
}
