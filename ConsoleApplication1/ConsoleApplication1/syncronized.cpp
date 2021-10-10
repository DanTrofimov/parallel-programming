#include <iostream>
#include <omp.h>
#include "windows.h"

void tenth()
{
	int d[6][8];
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			d[i][j] = rand() % 10;
		}
	}
	int max = -1;
	int min = 10;
	#pragma omp parallel for num_threads(5)
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (d[i][j] >= min && d[i][j] <= max) {
				continue;
			}
			#pragma omp critical 
			{
				if (d[i][j] < min) min = d[i][j];
				if (d[i][j] > max) max = d[i][j];
			}
		}
	}
	printf("Max value: %d, Min value: %d \n", max, min);
}

void eleventh()
{
	int a[30];
	int count = 0;
	for (int i = 0; i < 30; i++)
	{
		a[i] = rand() % 10;
	}
	#pragma omp parallel for num_threads(5)
	for (int i = 0; i < 30; i++)
	{
		if (a[i] % 9 == 0)
		{
			#pragma omp atomic
			count++;
		}
	}
	printf("Nums mod 9 = 0: %d", count);
}

void twelveth()
{
	int a[100];
	for (int i = 0; i < 100; i++)
	{
		a[i] = rand() % 10;
	}
	int max = -1;
	#pragma omp parallel for num_threads(5)
	for (int i = 0; i < 100; i++)
	{
		if (a[i] % 7 == 0)
		{
			#pragma omp critical
			{
				if (max < a[i]) max = a[i];
			}
		}
	}
	printf("Max mod 7 = 0: %d", max);
}

void thirteenth()
{
	#pragma omp parallel num_threads(8)
	{
		int threads_amount = omp_get_num_threads();
		for (int i = threads_amount - 1; i >= 0; i--)
		{
			#pragma omp barrier
			{
				if (i == omp_get_thread_num())
				{
					printf("Current thread: %d \n", omp_get_thread_num());
				}
			}
		}
	}
}

// another solutions of 13th
void thirteenth_2()
{
#pragma omp parallel num_threads(8)
	{
		Sleep(1000 / (omp_get_thread_num() + 1));
		printf("Current thread: %d \n", omp_get_thread_num() );
	}
}

void thirteenth_3()
{
#pragma omp parallel for schedule(static, 1)
	for (int i = omp_get_num_threads() - 1; i >= 0; i--)
	{
		Sleep(100 * i);
		printf("Current thread: %d \n", omp_get_thread_num());
	}
}

void thirteenth_4()
{
	int remaining_threads = 7;
	boolean is_continue;
	#pragma omp parallel num_threads(8)
	{
		while (is_continue)
		{
			if (remaining_threads == omp_get_thread_num())
			{
				printf("Current thread: %d\n", omp_get_thread_num());
				remaining_threads--;
			}
			if (remaining_threads == 0) {
				is_continue = false;
			}
		}
	}
}

void delay_helper(int delay, int counter) {
	if (counter >= 0) {
		Sleep(delay * counter);
		printf("Current thread: %d\n", omp_get_thread_num());
	}
}


void thirteenth_5()
{
	int current = 8;
	#pragma omp parallel sections num_threads(8)
	{
		#pragma omp section 
		{
			delay_helper(100, current - 1);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 2);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 3);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 4);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 5);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 6);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 7);
		}
		#pragma omp section 
		{
			delay_helper(100, current - 8);
		}
	}
}

int main()
{
	thirteenth_5();
}