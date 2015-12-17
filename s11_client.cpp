#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <map>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

using std::string;
using std::map;
using std::cout;
using std::cin;

/*
 * 1. Можно было бы передавать ip-сервера параметром командной строки.
 * Ваша программа действительно может общаться по сети, а не только в пределах одной машины.
 * 2. У вашего редактора же есть автодополнения: не нужно сокращать всё, что длиннее 3х символов: смысл некоторых сокращений не очевиден, по крайней мере, сходу: diep, si_server, slen.
 * Некоторые вещи понять можно только, если посмотреть как используются.
 * 
 * Нормально. Засчитано. Надо будет кратко рассказать, что знаете про сетевое взаимодействие и, в частности, про udp. 
 */

#define SRV_IP "127.0.0.1"

void diep(char *s)
{
	perror(s);
	exit(1);
}

sockaddr_in si_server;
int soc, slen = sizeof(si_server);
char buf[BUFLEN];

void start_receiver()
{
	pid_t pid = fork();
	if(pid != 0)
		return;
	while(1)
	{
		if(recvfrom(soc, buf, BUFLEN, 0, (sockaddr*)&si_server, (socklen_t*)&slen) == -1)
			diep("recvfrom()");
		cout << buf + 1 << "\n";
	}
}

void start_messenger()
{
	pid_t pid = fork();
	if(pid != 0)
		return;
	while(1)
	{
		string message;
		getline(cin, message);
		strcpy(buf + 1, message.c_str());
		buf[message.size() + 1] = 0;
		if(sendto(soc, buf, BUFLEN, 0, (sockaddr*)&si_server, slen) == -1)
			diep("sendto()");
	}
}

int main()
{
	memset((char*)&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(PORT);
	if(inet_aton(SRV_IP, &si_server.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	if((soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		diep("socket");
	
	cout << "Hello! Please, enter your nickname: ";
	string name;
	cin >> name;
	
	strcpy(buf + 1, name.c_str());
	if(sendto(soc, buf, BUFLEN, 0, (sockaddr*)&si_server, slen) == -1)
		diep("sendto()");
	cout << "Nice to meet you, " << name << "! Feel free to enter the conversation. Just type your message.\n";
	getline(cin, name);
	buf[0] = 1;
	start_receiver();
	start_messenger();
	while(1);
}
