#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

void generate_string(int n, char* string)
{
	if(n == 1)
	{	
		string[0] = 'a';
		return;
	}
	int m = 1 << (n - 1);
	string[m - 1] = 'a' + n - 1;
	generate_string(n - 1, string);
	generate_string(n - 1, string + m);
}

int main()
{
	int n;
	scanf("%d", &n);
	char *ans = (char*)malloc((1 << n));
	generate_string(n, ans);
	puts(ans);
	free(ans);
    return 0;
}
