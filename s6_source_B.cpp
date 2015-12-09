#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <string.h>

using std::string;
using std::cout;
using std::ifstream;

int main()
{
	key_t key = ftok("shm_size", 0);
	int shmid = shmget(key, sizeof(int), 0666);
	
	int *len = (int*) shmat(shmid, NULL, 0);
	int source_size = *len;
	shmdt(len);
	
	key = ftok("shm_source", 0);
	shmid = shmget(key, source_size, 0666);
	
	char *source = (char*) shmat(shmid, NULL, 0);
	cout << source << "\n";
	shmdt(source);
	
	
	return 0;
}
