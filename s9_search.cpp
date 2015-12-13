#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>

using std::cout;
using std::string;

void search(string path, int depth, string needle)
{
	if(depth < 0)
		return;
	cout << "Search in \"" << path << "\" for \"" << needle << "\". Allowed depth: " << depth << "\n";
	DIR *dir = opendir(path.c_str());
	dirent *file;
	while(file = readdir(dir))
	{
		if(string(file->d_name) == "." || string(file->d_name) == "..")
			continue;
		if(file->d_name == needle)
		{
			cout << "File found at \"" << path << "\"!\n";
			exit(0);
		}
		struct stat info;
		stat((path + "/" + file->d_name).c_str(), &info);
		if(S_ISDIR(info.st_mode))
			search(path + "/" + file->d_name, depth - 1, needle);
	}
	closedir(dir);
}
 
int main(int argc, char *argv[])
{
	string start_dir = argv[1];
	int depth = atoi(argv[2]);
	string needle = argv[3];
	search(start_dir, depth, needle);
	cout << "File not found!\n";
	return 0;
}
