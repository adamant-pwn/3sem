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

void diep(char *s)
{
	perror(s);
	exit(1);
}

struct data
{
	string name;
	sockaddr_in si_receiver;
	
	data(){}
	data(string name, sockaddr_in si_receiver):name(name), si_receiver(si_receiver){}
};

int main(void)
{
	cout << "I am the server and I'm going to deliver some messages!\n";
	sockaddr_in si_me, si_sender;
	int soc, slen = sizeof(si_sender);
	char buf[BUFLEN];

	if((soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		diep("socket");

	memset((char*)&si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(soc, (sockaddr*)&si_me, sizeof(si_me)) == -1)
		diep("bind");
		
	map<string, data> clients;
	while(1)
	{
		if(recvfrom(soc, buf, BUFLEN, 0, (sockaddr*)&si_sender, (socklen_t*)&slen) == -1)
			diep("recvfrom()");
		string message = buf + 1;
		string sender_ip = inet_ntoa(si_sender.sin_addr) + string(":") + std::to_string(ntohs(si_sender.sin_port));
		if(buf[0] == 0) // IP is telling its nickname
		{
			cout << sender_ip << " is registered as " << message << "\n";
			clients[sender_ip] = data(message, si_sender);
		}
		else // IP is sending a message
		{
			cout << clients[sender_ip].name << " is sending a message \"" << message << "\".\n";
			message = clients[sender_ip].name + ": " + message;
			strcpy(buf + 1, message.c_str());
			buf[message.size() + 1] = 0;
			for(auto client: clients)
			{
				string ip = client.first;
				if(ip != sender_ip)
				{
					auto si_receiver = client.second.si_receiver;
					if(sendto(soc, buf, BUFLEN, 0, (sockaddr*)&si_receiver, slen) == -1)
						diep("sendto()");
				}
			}
		}
	}
	return 0;
}
