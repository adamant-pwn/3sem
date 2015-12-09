#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

const int N = 1000;
int A[N][N], B[N][N], C[N][N];

using std::cout;

void *multiply(void *row)
{
	int i = *(int*)row;
	for(int j = 0; j < N; j++)
		for(int k = 0; k < N; k++)
			C[i][j] += A[i][k] * B[k][j];
}

int main()
{
	// RANDOM OVER 9000 CONFIRMED BY FAIR DICE ROLL
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			A[i][j] = i + j + 1;
			
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			B[i][j] = i * j + 1;
	// ---------------------------
	
	#define FAIR 0
	#if FAIR
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			for(int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
	/*
	 *	real 	0m6.356s
	 *	user 	0m6.348s
	 *	sys 	0m0.008s 
	 * */
	#else
	pthread_t id[N];
	int rows[N];
	for(int i = 0; i < N; i++)
	{
		rows[i] = i;
		pthread_create(&id[i], NULL, multiply, &rows[i]);
	}
	
	for(int i = 0; i < N; i++)
		pthread_join(id[i], NULL);
	/*
	 *	real 	0m2.468s
	 *	user 	0m9.524s
	 *	sys 	0m0.036s 
	 * */
	#endif
    return 0;
}
