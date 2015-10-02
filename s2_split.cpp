#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

vector<string> split(string str, string delim)
{
	vector<string> ret;
	string cur;
	for(auto it: str)
	{
		if(count(begin(delim), end(delim), it) == 0)
		{
			cur += it;
		}
		else
		{
			if(!cur.empty())
				ret.push_back(cur);
			cur.clear();
		}
	}
	return ret;
}

int main()
{
	string str, delim;
	getline(cin, str);
	getline(cin, delim);
	for(auto it: split(str, delim))
		cout << it << "\n";
}
