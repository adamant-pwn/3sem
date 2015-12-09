#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using std::string;
using std::stringstream;
using std::vector;
using std::cin;
using std::cout;

struct Task
{
	int time;
	string name;
	vector<string> argv;
};

void scan_task(Task &task)
{
	cin >> task.time;
	cin >> task.name;
	task.argv.push_back(task.name);
	string tmp;
	getline(cin, tmp);
	stringstream parser(tmp);
	while(parser >> tmp)
		task.argv.push_back(tmp);
}

int start_task(const Task &task)
{
	vector<char*> argv;
	for(auto &it: task.argv)
		argv.push_back((char*)it.data());
	argv.push_back(NULL);
	pid_t pid = fork();
	if(pid == 0)
	{
		sleep(task.time);
		cout << "Process " << getpid() << " started.\n";
		execvp(task.name.data(), argv.data());
	}
	return pid;
}

int main()
{
	int task_count;
	cin >> task_count;
	vector<int> task_pids;
	for(int i = 0; i < task_count; i++)
	{
		Task task;
		scan_task(task);
		task_pids.push_back(start_task(task));
	}
	for(auto task: task_pids)
	{
		int status;
		waitpid(task, &status, 0);
		cout << "Process " << task << " terminated with exit status " << status << ".\n";
	}
}
