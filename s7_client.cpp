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

struct mymsgbuf
{
	long type;
	int num[2];
};

const int maxn = 10;

int buy_id(int msgid)
{
	mymsgbuf mybuf;
	mybuf.type = 1;
	msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
	msgrcv(msgid, &mybuf, sizeof(mybuf.num), maxn + 1, 0);
	return mybuf.num[0];
}

int main()
{
	int msgid; 
	key_t key = ftok("shared_memory", 0); 

	mymsgbuf mybuf;
  
	msgid = msgget(key, 0666 | IPC_CREAT);
	
	cout << "Enter queries. Type -1 -1 to exit.\n";
	while(1)
	{
		int a, b;
		cin >> a >> b;
		if(a == -1 && b == -1)
			break;
		
		// Клиент "покупает" себе id для запроса на сервер. Таким образом запросы разных клиентов не смешиваются.
		int query_id = buy_id(msgid);
		mybuf.num[0] = a;
		mybuf.num[1] = b;
		mybuf.type = query_id;
		msgsnd(msgid, &mybuf, sizeof(mybuf.num), 0);
		msgrcv(msgid, &mybuf, sizeof(mybuf.num), query_id + maxn, 0);
		cout << "Your ID: " << query_id << ". Your answer: " << mybuf.num[0] << "\n";
	}
	cout << "Good bye!\n";
	return 0;
} 
