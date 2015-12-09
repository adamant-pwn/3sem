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

int main()
{
	int msgid; 
	key_t key = ftok("shared_memory", 0); 

	struct mymsgbuf
	{
		long type;
		int num;
	} mybuf;
  
	msgid = msgget(key, 0666 | IPC_CREAT);
	
	cout << "Enter queries. Type -1 to exit.\n";
	while(1)
	{
		int num;
		cin >> num;
		if(num == -1)
			break;
		mybuf.type = 1;
		mybuf.num = num;
		msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
		msgrcv(msgid, &mybuf, sizeof(mybuf.num), 2, 0);
		cout << num << "! = " << mybuf.num << "\n";
	}
	cout << "Good bye!\n";
	mybuf.type = 1;
	mybuf.num = -1;
	msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);

	return 0;
} 
