#include <iostream>
#include <vector>
#include <unistd.h>
#include <wait.h>

using std::vector;
using std::cin;
using std::cout;

void build_process_tree(int vertex, int parent, vector<vector<int>> &tree)
{
	for(auto child: tree[vertex])
		if(child != parent)
		{
			pid_t pid = fork();
			if(pid == 0)
			{
				cout << "Child process " << getpid() << " created. Vertex = " << child + 1 << "\n";
				build_process_tree(child, vertex, tree);
			}
			else
			{
				int status;
				waitpid(pid, &status, 0);
				cout << "Child process " << pid << " terminated with exit status " << status << ". Vertex = " << child + 1 << "\n";
				continue;
			}
		}
	exit(0);
}

int main()
{
	int tree_size;
	cin >> tree_size;
	vector<vector<int>> tree(tree_size);
	for(int i = 0; i < tree_size - 1; i++)
	{
		int a, b;
		cin >> a >> b;
		tree[a - 1].push_back(b - 1);
		tree[b - 1].push_back(a - 1);
	}
	build_process_tree(0, 0, tree);
}
