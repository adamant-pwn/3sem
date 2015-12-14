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

template<class T>class locked_vector : public vector<T>
{
	std::mutex vector_mutex;
	public:
	void safety_push_back(T elem)
	{
		vector_mutex.lock();
		this->push_back(elem);
		vector_mutex.unlock();
	}
	
	int safety_pop()
	{
		vector_mutex.lock();
		int ret = this->back();
		this->pop_back();
		vector_mutex.unlock();
		return ret;
	}
};

locked_vector<int> available_ids;

key_t key = ftok("shared_memory", 0); 
int msgid = msgget(key, 0666 | IPC_CREAT);

struct msg_t
{
	long type;
	int num[2];
} query[maxn];

void *answer_query(void *arg)
{
	msg_t msg = *(msg_t*)arg;
	msg.type += maxn;
	msg.num[0] *= msg.num[1];
	msgsnd(msgid, &msg, sizeof(msg.num), 0);
	available_ids.safety_push_back(msg.type - maxn);
}
	
int main()
{
	cout << "Hello! I'm the server and I'm ready to process your queries!\n";

	msg_t msg;
	
	for(int i = 3; i < maxn; i++)
		available_ids.safety_push_back(i);
	
	while(1)
	{
		msgrcv(msgid, &msg, sizeof(msg.num), -maxn, 0);
		if(msg.type == 1) // client is asking for id
		{
			msg.type = maxn + 1;
			while(available_ids.empty()); // wait for some thread to free the ID
			msg.num[0] = available_ids.safety_pop();
			msgsnd(msgid, &msg, sizeof(msg.num), 0);
		}
		else // client is asking for query answer
		{
			query[msg.type] = msg;
			pthread_t id;
			pthread_create(&id, NULL, answer_query, &query[msg.type]);
		}
	}
	
	cout << "Good bye!\n";
	msgctl(msgid, IPC_RMID, NULL);
	return 0;
} 
