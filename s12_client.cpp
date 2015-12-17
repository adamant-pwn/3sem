#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <pthread.h>

using std::string;
using std::cout;
using std::cin;

string name;
const int max_len = 200;
char message[max_len + 1];

void *start_receiver(void *arg)
{
	int sockfd = *(int*)arg;
	while(1)
	{
		read(sockfd, message, max_len);
		cout << message << "\n";
	}
}

void *start_sender(void *arg)
{
	int sockfd = *(int*)arg;
	while(1)
	{
		string msg;
		getline(cin, msg);
		msg = name + ": " + msg;
		strcpy(message, msg.c_str());
		write(sockfd, message, max_len);
	}
}

int main(int argc, char **argv)
{
	string ip;
	if(argc == 2)
		ip = argv[1];
	cout << "Hello! Enter your name, please: ";
	getline(cin, name);
	cout << "Welcome to the chat! Feel free to talk here.\n";
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in servaddr;
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	inet_aton(ip.c_str(), &servaddr.sin_addr);
	
	connect(sockfd, (sockaddr*) &servaddr, sizeof(servaddr));
	pthread_t id;
	pthread_create(&id, NULL, start_sender, &sockfd);
	pthread_create(&id, NULL, start_receiver, &sockfd);
	while(not false);
}
