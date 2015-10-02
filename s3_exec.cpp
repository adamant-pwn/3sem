#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using namespace std;

int main()
{
	freopen("input.txt", "r", stdin);
	int n;
	cin >> n;
	for(int i = 0; i < n; i++)
	{
		int time;
		cin >> time;
		
		string name;
		cin >> name;
		
		string tmp;
		getline(cin, tmp);
		stringstream sstream;
		sstream << tmp;
		vector<string> argv_v(1, name);
		while(sstream >> tmp)
			argv_v.push_back(tmp);
		int n = argv_v.size();
		char **argv = (char**)malloc((n + 1) * sizeof(char*));
		for(int i = 0; i < n; i++)
		{
			argv[i] = (char*)malloc(1 + argv_v[i].size());
			memcpy(argv[i], argv_v[i].c_str(), argv_v[i].size());
		}
		argv[n] = NULL;
		pid_t pid = fork();
		if(pid == 0)
		{
			sleep(time);
			execvp(name.c_str(), argv);
		}
		else
		{
			for(int i = 0; i <= n; i++)
				free(argv[i]);
			free(argv);
		}
	}
}
