#include <iostream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using namespace std;

vector<vector<int>> g;

void dfs(int v = 0, int p = 0)
{
	for(auto u: g[v])
		if(u != p)
		{
			pid_t pid = fork();
			if(pid == 0)
			{
				cout << "Child process " << getpid() << " created. Vertex = " << u + 1 << "\n";
				dfs(u, v);
			}
			else
			{
				int status;
				waitpid(pid, &status, 0);
				cout << "Child process " << pid << " terminated with exit status " << status << ". Vertex = " << u + 1 << "\n";
				continue;
			}
		}
	exit(0);
}

int main()
{
	int n;
	cin >> n;
	g.resize(n);
	/*
	 * edge list, 1-based. Numbers of vertices from 1 to n.
	 * root = 1
	 * */
	for(int i = 0; i < n - 1; i++)
	{
		int a, b;
		cin >> a >> b;
		g[a - 1].push_back(b - 1);
		g[b - 1].push_back(a - 1);
	}
	dfs();
}
