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
	string my_source;
	ifstream in("s6_source_A.cpp");
	string temp;
	while(getline(in, temp))
		my_source += "\n" + temp;
	int source_size = my_source.size();
	
	key_t key = ftok("shm_source", 0);
	int shmid = shmget(key, source_size, 0666 | IPC_CREAT);
	
	char *source = (char*) shmat(shmid, NULL, 0);
	memcpy(source, my_source.data(), source_size);
	shmdt(source);
	
	key = ftok("shm_size", 0);
	shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
	
	int *len = (int*) shmat(shmid, NULL, 0);
	*len = source_size;
	shmdt(len);
	
	return 0;
}
