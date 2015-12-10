#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <map>
#include <vector>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <deque>
#include <assert.h>
#include <sys/msg.h>

using std::cout;
using std::string;
using std::map;
using std::vector;
using std::deque;

/* * * /
 * Участвуют четыре процесса - два "стола", выступающих в роли сервера и
 * двое "рабочих", выступающих в роли клиентов. Они обращаются к
 * столам, чтобы брать или забирать тарелки.
 * */

const string filename = "shm_table";

int communicate_type;
sembuf enter, leave; 

struct dish
{
	int num;
	int time[2];
};

struct mymsgbuf
{
	long type;
	dish data;
};


map<string, dish> dishes;

const int table_limit = 10;

void *table(void *arg)
{
	int type = *(int*)arg;
	dish dishes[table_limit];
	deque<int> free_pos;
	deque<int> busy_pos;
	for(int i = 0; i < table_limit; i++)
		free_pos.push_back(i);
	
	key_t key = ftok((filename + std::to_string(type)).c_str(), 0);
	
	if(communicate_type == 0)
	{ // Разделяемая память с семафорами -------------------------
		int shmid = shmget(key, sizeof(dish), 0666);
		int semid = semget(key , 1, 0666);
		dish *query = (dish*) shmat(shmid, NULL, 0);
		while(true)
		{
			semop(semid, &enter, 1);
			if(query[0].num != 0)
			{
				if(!free_pos.empty())
				{
					dishes[free_pos.front()] = query[0];
					query[0].num = 0;
					busy_pos.push_back(free_pos.front());
					free_pos.pop_front();
				}
			}
			int last_time = -1;
			if(query[1].num == 0)
			{
				if(!busy_pos.empty())
				{
					query[1] = dishes[busy_pos.front()];
					last_time = query[1].time[0];
					free_pos.push_back(busy_pos.front());
					busy_pos.pop_front();
				}
			}
			semop(semid, &leave, 1);
			if(last_time == 0)
				break;
		}
	} // ---------------------------------------------------
	
	else
	{ // Очереди сообщений ---------------------------------
		int msgid = msgget(key, 0666);
		while(true)
		{
			mymsgbuf mybuf;
			msgrcv(msgid, &mybuf, sizeof(mybuf.data), 1, 0);
			if(free_pos.empty() || mybuf.data.num == -42)
			{
				msgsnd(msgid, &mybuf, sizeof(mybuf.data), 0);
			}
			else
			{
				dishes[free_pos.front()] = mybuf.data;
				busy_pos.push_back(free_pos.front());
				free_pos.pop_front();
			}
			int last_time = -1;
			if(!busy_pos.empty())
			{
				mybuf.type = 2;
				mybuf.data = dishes[busy_pos.front()];
				last_time = mybuf.data.time[0];
				free_pos.push_back(busy_pos.front());
				busy_pos.pop_front();
				msgsnd(msgid, &mybuf, sizeof(mybuf.data), 0);
				if(last_time == 0)
					break;
			}
		}
	} // ---------------------------------------------------
}

void put_dish(dish cur, int type)
{
	if(type == 2)
		return;
	key_t key = ftok((filename + std::to_string(type)).c_str(), 0);
	if(communicate_type == 0)
	{ // Разделяемая память с семафорами -------------------------
		int shmid = shmget(key, sizeof(dish), 0666);
		int semid = semget(key , 1, 0666);
		dish *query = (dish*) shmat(shmid, NULL, 0);
		while(query[0].num != 0);
		semop(semid, &enter, 1);
		query[0] = cur;
		semop(semid, &leave, 1);
	} // ---------------------------------------------------
	
	else
	{ // Очереди сообщений ---------------------------------
		int msgid = msgget(key, 0666);
		mymsgbuf mybuf;
		mybuf.type = 1;
		mybuf.data = cur;
		msgsnd(msgid, &mybuf, sizeof(mybuf.data), 0);
	} // ---------------------------------------------------
	
}

dish take_dish(int type)
{
	key_t key = ftok((filename + std::to_string(type)).c_str(), 0);
	
	if(communicate_type == 0)
	{ // Разделяемая память с семафорами -------------------------
		int shmid = shmget(key, sizeof(dish), 0666);
		int semid = semget(key , 1, 0666);
		dish *query = (dish*) shmat(shmid, NULL, 0);
		dish cur;
		while(query[1].num == 0);
		semop(semid, &enter, 1);
		cur = query[1];
		query[1].num = 0;
		semop(semid, &leave, 1);
		return cur;
	} // ---------------------------------------------------
	
	else
	{ // Очереди сообщений ---------------------------------
		int msgid = msgget(key, 0666);
		mymsgbuf mybuf;
		msgrcv(msgid, &mybuf, sizeof(mybuf.data), 2, 0);
		return mybuf.data;
	} // ---------------------------------------------------
}

void process(dish cur, int type)
{
	sleep(cur.time[type]);
	put_dish(cur, type + 1);
}

void *worker(void *arg)
{
	int type = *(int*)arg;
	while(true)
	{
		dish cur = take_dish(type);
		process(cur, type);
		if(cur.time[0] == 0)
			break;
		cout << "Dish #" << cur.num << " is " << (type ? "wiped" : "washed") << " and put on the table.\n";
	}
}

void init()
{
	if(communicate_type == 0)
	{
		enter.sem_op = -1;
		leave.sem_op = 1;
		for(int type = 0; type <= 1; type++)
		{
			key_t key = ftok((filename + std::to_string(type)).c_str(), 0);
			int shmid = shmget(key, sizeof(dish), 0666 | IPC_CREAT);
			int semid;
			semid = semget(key , 1, 0666 | IPC_CREAT);
			semctl(semid, IPC_RMID, NULL);
			semid = semget(key , 1, 0666 | IPC_CREAT);
			dish *query = (dish*) shmat(shmid, NULL, 0);
			query[0].num = query[1].num = 0;
			semop(semid, &leave, 1);
		}
	}
	else
	{
		for(int type = 0; type <= 1; type++)
		{
			key_t key = ftok((filename + std::to_string(type)).c_str(), 0);
			int msgid = msgget(key, 0666 | IPC_CREAT);
			mymsgbuf mybuf;
			mybuf.type = 0;
			mybuf.data.num = -42;
			msgsnd(msgid, &mybuf, sizeof(mybuf.data), 0);
		}
	}
}
 
int main(int argc, char *argv[])
{
	communicate_type = atoi(argv[1]);
	init();
	
	pthread_t id[4];
	int washer = 0, wiper = 1;
	int dirt = 0, clean = 1;
	pthread_create(&id[0], NULL, worker, &washer);
	pthread_create(&id[1], NULL, worker, &wiper);
	pthread_create(&id[2], NULL, table, &dirt);
	pthread_create(&id[3], NULL, table, &clean);
	std::ifstream d_type_list("type.txt");
	std::ifstream d_query_list("query.txt");
	string dish_name, colon;
	int wash_time, wipe_time;
	int amount;
	while(d_type_list >> dish_name >> colon >> wash_time >> wipe_time)
	{
		dishes[dish_name].time[0] = wash_time;
		dishes[dish_name].time[1] = wipe_time;
	}
	int counter = 1;
	while(d_query_list >> dish_name >> colon >> amount)
	{
		for(int i = 0; i < amount; i++)
		{
			dish cur = dishes[dish_name];
			cur.num = counter++;
			put_dish(cur, 0);
		}
	}
	dish finish;
	finish.num = -1;
	finish.time[0] = finish.time[1] = 0;
	put_dish(finish, 0);
	for(int i = 0; i < 4; i++)
		pthread_join(id[i], 0);
	cout << "All dishes are washed and wiped! Good bye!\n";
	return 0;
}
