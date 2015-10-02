#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Совсем не круто подключать все пространство имён целиком.
 * Вы из него ничего не используете.
 * Обычно если используют пару классов, то пишут их отдельно:
 * using std::vector;
 * using std::string;
 */

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
