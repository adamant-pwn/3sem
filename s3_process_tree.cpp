#include <iostream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using namespace std;

vector<vector<int>> g;

/*
 * Дополняйте задачи примерами входных данных в отдельном файле, 
 * которые я бы запускал через перенаправление потока ввода: ./a.out << input.txt
 * 
 * Именование переменных никуда не годится. Представьте, что вашей ф-ей запуска необходимо будет кому-то воспользоваться, либо надо будет
 * добавить в код какую-то фичу. Для этого стороннему человеку надо будет в коде разобраться и причём быстро.
 * Поэтому надо объявлять переменные и называть ф-и максимально понятно - об однобуквенных переменных стоит сразу забыть (возможно только i,j,k для индексов
 * в массиве и то лучше, что-то более осмысленное придумать).
 * Ф-я запускает процессы, поэтому это необходимо отразить в названии, а не название обхода, который вы используете.
 */

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
