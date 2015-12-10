#include <sys/types.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int counter;
key_t key; 
int semid;
const int bound = 1e6;


void *add(void* arg)
{
	
	sembuf mybuf; 
	mybuf.sem_flg = 0;
	mybuf.sem_num = 0;
	bool type = *(bool*)arg;
	for(int i = 0; i < bound; i++)
	{
		mybuf.sem_op = -1;
		semop(semid, &mybuf, 1);
		counter++;
		mybuf.sem_op = 1;
		semop(semid, &mybuf, 1);
		
	}
}

int main() 
{
	key = ftok("shared_memory", 0); 
	semid = semget(key , 1, 0666 | IPC_CREAT);
	semctl(semid, IPC_RMID, NULL);
	semid = semget(key , 1, 0666 | IPC_CREAT);
	sembuf mybuf; 
	mybuf.sem_flg = 0;
	mybuf.sem_num = 0;
	mybuf.sem_op = 1;
	semop(semid, &mybuf, 1);
	
	bool tp[2] = {0, 1};
	pthread_t id[2];
	pthread_create(&id[0], NULL, add, &tp[0]);
	pthread_create(&id[1], NULL, add, &tp[1]);
	pthread_join(id[0], NULL);
	pthread_join(id[1], NULL);
	semctl(semid, IPC_RMID, NULL);
	std::cout << "Final counter value: " << counter << "\n";
	std::cout << "Expected: " << 2 * bound << "\n";
	return 0;
}
