#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#define LAST_MESSAGE 255 

using std::cout;
using std::cin;
using std::vector;

const int maxn = 10;
vector<int> available;

key_t key = ftok("shared_memory", 0); 
int msgid = msgget(key, 0666 | IPC_CREAT);

struct mymsgbuf
{
	long type;
	int num[2];
} query[maxn];

void *thread(void *arg)
{
	mymsgbuf mybuf = *(mymsgbuf*)arg;
	mybuf.type += maxn;
	mybuf.num[0] *= mybuf.num[1];
	msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
	/*
	 * Метод push_back не thread-safety, поэтому такой вариант не подходит.
	 */
	available.push_back(mybuf.type - maxn);
}
	
int main()
{
	cout << "Hello! I'm the server and I'm ready to process your queries!\n";

	mymsgbuf mybuf;
	
	for(int i = 3; i < maxn; i++)
		available.push_back(i);
	
	while(1)
	{
		msgrcv(msgid, &mybuf, sizeof(mybuf.num), -maxn, 0);
		if(mybuf.type == 1)
		{
			while(available.empty());
			mybuf.type = maxn + 1;
			mybuf.num[0] = available.back();
			available.pop_back();
			msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
		}
		else
		{
			query[mybuf.type] = mybuf;
			pthread_t id;
			pthread_create(&id, NULL, thread, &query[mybuf.type]);
		}
	}
	
	cout << "Good bye!\n";
	msgctl(msgid, IPC_RMID, NULL);
	return 0;
} 
