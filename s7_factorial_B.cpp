#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define LAST_MESSAGE 255 

using std::cout;
using std::cin;

int fact(int n)
{
	return n ? n * fact(n - 1) : 1;
}

int main()
{
	cout << "Hello! I'm the queue and I'm ready to process your queries!\n";
  
	int msgid; 
	key_t key = ftok("shared_memory", 0); 

	struct mymsgbuf
	{
		long type;
		int num;
	} mybuf;
  
	msgid = msgget(key, 0666 | IPC_CREAT);
	
	while(1)
	{
		msgrcv(msgid, &mybuf, sizeof(mybuf.num), 1, 0);
		mybuf.type = 2;
		if(mybuf.num == -1)
			break;
		mybuf.num = fact(mybuf.num);
		msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
	}
	
	cout << "Good bye!\n";
	msgctl(msgid, IPC_RMID, NULL);
	return 0;
} 
