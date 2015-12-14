#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <mutex>

#define LAST_MESSAGE 255 

using std::cout;
using std::cin;
using std::vector;

const int maxn = 10;


class locked_vector : vector<int>
{
	std::mutex vector_mutex;
	public:
	void safety_push_back(int elem)
	{
		vector_mutex.lock();
		this->push_back(elem);
		vector_mutex.unlock();
	}
	
	int safety_pop()
	{
		while(this->empty());
		vector_mutex.lock();
		int ret = this->back();
		this->pop_back();
		vector_mutex.unlock();
		return ret;
	}
};

locked_vector available;

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
	available.safety_push_back(mybuf.type - maxn);
}
	
int main()
{
	cout << "Hello! I'm the server and I'm ready to process your queries!\n";

	mymsgbuf mybuf;
	
	for(int i = 3; i < maxn; i++)
		available.safety_push_back(i);
	
	while(1)
	{
		msgrcv(msgid, &mybuf, sizeof(mybuf.num), -maxn, 0);
		if(mybuf.type == 1)
		{
			mybuf.type = maxn + 1;
			mybuf.num[0] = available.safety_pop();
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
