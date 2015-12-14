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

/*
 * В целом, код рабочий, но многое не нравится:
 * 1) У вас locked_vector - это вектор интов, что никак не отражено в названии. Если вы пишите на С++, что 
 * вам мешало сделать locked_vector также шаблонным.
 * 2) while(this->empty()); ? бросайте исключения, падайте на assert`e ... Зависнуть в бесконечном цикле - вариант только на временя отладки. Отладили - удалите.
 * 3) available - это прилагательное. в названии переменных и структур должно быть существительное.
 * 4) mymsgbuf - это было написано в примере из учебника. на учебник не надо ровняться с точки зрения стиля написания кода. Из названия этой структуры совершенно неясно, зачем она нужна.
 * 5) void *thread(void *arg) - в названии ф-й и методов должен быть глагол.
 * 6) по возможности надо разбивать код на небольшие ф-и с понятными названиями, что делает код самодокументируемым.
 * 
 * Если бы зачёты не надо было проставить до 19 числа, то код надо было бы явно править.
 * Пока пусть так.
 */

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
