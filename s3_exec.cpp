#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using std::string;
using std::stringstream;
using std::vector;
using std::cin;
using std::cout;
using std::allocator;

void scan_task(int &time, string &name, vector<char*> &argv)
{
	cin >> time;
	cin >> name;
	argv.push_back((char*)malloc(1 + name.size()));
	memcpy(argv.back(), name.c_str(), name.size());
	
	string tmp;
	getline(cin, tmp);
	stringstream parser(tmp);
	while(parser >> tmp)
	{
		argv.push_back((char*)malloc(1 + tmp.size()));
		memcpy(argv.back(), tmp.c_str(), tmp.size());
	}
	argv.push_back(NULL);
}

int start_task(int time, string &name, vector<char*> &argv)
{	
	pid_t pid = fork();
	if(pid == 0)
	{
		sleep(time);
		cout << "Process " << getpid() << " started.\n";
		execvp(name.c_str(), allocator<char*>().address(*argv.begin()));
	}
	return pid;
}

int main()
{
	int task_count;
	cin >> task_count;
	vector<int> wait;
	for(int i = 0; i < task_count; i++)
	{
		int time;
		string name;
		vector<char*> argv;
		scan_task(time, name, argv);
		wait.push_back(start_task(time, name, argv));
	}
	for(auto task: wait)
	{
		int status;
		waitpid(task, &status, 0);
		cout << "Process " << task << " terminated with exit status " << status << ".\n";
	}
}
