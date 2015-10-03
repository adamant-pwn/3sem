#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

using std::string;
using std::stringstream;
using std::vector;
using std::cin;
using std::cout;
using std::allocator;

/*
 * надо завести структуру Task, где будут поля time, name, argv.
 * просто без контекста неясно, что за name, time ... вы хотите считать
 */

void scan_task(int &time, string &name, vector<char*> &argv)
{
	cin >> time;
	cin >> name;
/*
 * что именно вы вкладываете в перенос 1 в первый аргумент сложения. выглядит непривычно.
 */
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
		execvp(name.c_str(), argv.data());
	}
	return pid;
}

int main()
{
	int task_count;
	cin >> task_count;
  
  /*
   * Глаголами нужно называть только ф-и. Переменные - существительные.
   * Кого ждать?
   */
	vector<int> wait;
	for(int i = 0; i < task_count; i++)
	{
		int time;
		string name;
		vector<char*> argv;
		scan_task(time, name, argv);
/*
 * Я имел ввиду, что вы создаете как и раньше std::vector< std::string > parameters; и считываете в него.
 * Перед тем, как вызвать exec, создаете std::vector< char* > argv;
 * ...
 * argv[i] = parameters[i].data();
 * ...
 * т.е. просто конвертируете в то, что можно будет передать в exec, а в предыдущих местах работаете с std::string
 * Просто вы начали вручную выделять память malloc'ом, и ,конечно же, забыли её почистить.
 * А так бы она автоматически почистилась при вызове деструкторов.
 */
    wait.push_back(start_task(time, name, argv));
	}
	for(auto task: wait)
	{
		int status;
		waitpid(task, &status, 0);
		cout << "Process " << task << " terminated with exit status " << status << ".\n";
	}
}
