#include <iostream>
#include <omp.h>

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



int main()
{
	eleventh();
}

