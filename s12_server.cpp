#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

using std::cout;
using std::string; 
using std::pair;

const int max_connection_amount = 10, max_len = 200;
const string filename = "shared_memory";

struct msg_t
{
	long type;
	char text[max_len + 1];
};

key_t key;
int msgid;

void init()
{
	key_t key = ftok(filename.c_str(), 0); 
	msgid = msgget(key, 0666 | IPC_CREAT);
	msgctl(msgid, IPC_RMID, NULL);
	msgid = msgget(key, 0666 | IPC_CREAT);
	msg_t message;
	message.text[0] = -1;
	for(int i = 1; i <= max_connection_amount; i++)
	{
		message.type = i;
		msgsnd(msgid, &message, sizeof(message.text), 0);
	}
}

void *start_receiver(void *arg)
{
	msg_t message;
	pair<int, int> tmp = *(pair<int, int>*)arg;
	int num = tmp.first;
	int sockfd = tmp.second;
	char buf[max_len + 1];
	memset(buf, 0, sizeof(buf));
	while(1)
	{
		read(sockfd, buf, max_len);
		cout << num << ' ' << "RECEIVED!\n";
		cout << buf << "\n";
		memcpy(message.text + 1, buf, sizeof(buf));
		if(num > 1)
		{
			message.text[0] = '-';
			message.type = (num - 1);
			cout << "sent - " << num << ' ' << message.type << "\n";
			msgsnd(msgid, &message, sizeof(message.text), 0);
		}
		message.text[0] = '+';
		message.type = (num + 1);
		cout << "sent + " << num << ' ' << message.type << "\n";
		msgsnd(msgid, &message, sizeof(message.text), 0);
	}
}

void *start_sender(void *arg)
{
	msg_t message;
	pair<int, int> tmp = *(pair<int, int>*)arg;
	int num = tmp.first;
	int sockfd = tmp.second;
	char buf[max_len + 1];
	while(1)
	{
		msgrcv(msgid, &message, sizeof(message.text), num, 0);
		if(message.text[0] != -1)
			cout << num << ' ' << "READED " << message.text << "\n";
		else
		{
			msgsnd(msgid, &message, sizeof(message.text), 0);
			continue;
		}
		strcpy(buf, message.text + 1);
		write(sockfd, buf, max_len + 1);
		
		if(num > 1 && message.text[0] == '-')
		{
			message.type = (num - 1);
			msgsnd(msgid, &message, sizeof(message.text), 0);
		}
		else if(message.text[0] == '+')
		{
			message.type = (num + 1);
			msgsnd(msgid, &message, sizeof(message.text), 0);
		}
	}
}

void start_contacter(int num, int sockfd)
{
	pid_t pid = fork();
	if(pid != 0)
		return;
	pthread_t id;
	pair<int, int> arg{num, sockfd};
	pthread_create(&id, NULL, start_receiver, &arg);
	pthread_create(&id, NULL, start_sender, &arg);
	while(getppid() != 1);
}

int main()
{
	init();
	int sockfd, cli_sockfd;
	socklen_t clilen;
	sockaddr_in servaddr, cliaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd, (sockaddr*) &servaddr, sizeof(servaddr));
	listen(sockfd, max_connection_amount);
	for(int i = 1; i <= max_connection_amount; i++)
	{
		clilen = sizeof(cliaddr);
		cout << sockfd << ' ' << "ok\n";
		cli_sockfd = accept(sockfd, (sockaddr*) &cliaddr, &clilen);
		cout << "User #" << i << " connected.\n";
		cout << "Socket: " << cli_sockfd << "\n";
		start_contacter(i, cli_sockfd);
		close(cli_sockfd);
	}
	while(not false);
}
