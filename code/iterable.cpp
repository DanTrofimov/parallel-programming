#include <iostream>
#include <omp.h>

void sixth()
{
	int a[100];
	for (int i = 0; i < 100; i++)
	{
		a[i] = rand() % 10;
	}
	int for_sum = 0;
	#pragma omp parallel for
	for (int i = 0; i < 100; i++)
	{	
		for_sum += a[i];
	}	

	printf("Average for sum: %d \n", for_sum / 100);

	int sum = 0;
	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < 100; i++) {
			sum += a[i];
	}
	printf("Average for reduction sum : %d \n", sum / 100);
}

void seventh()
{
	int a[12];
	int b[12];
	int c[12];
	#pragma omp parallel for schedule(static, 5) num_threads(3)
	for (int i = 0; i < 12; i++)
	{
		a[i] = rand() % 10;
		b[i] = rand() % 10;
		printf("a = %d, b = %d, current thread = %d\n", a[i], b[i], omp_get_thread_num());
	}
	#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
	for (int i = 0; i < 12; i++)
	{
		c[i] = a[i] + b[i];
		printf("Sum: %d, current thread = %d\n", c[i], omp_get_thread_num());
	}
}

void eigth()
{
	long a[16000];
	double b[16000];

	for (int i = 0; i < 16000; i++) {
		a[i] = i;
	}

	int chunk_size = 300;

	double start_time = omp_get_wtime();
	#pragma omp parallel for schedule(dynamic, chunk_size)
	for (int i = 1; i < 15999; i++)
	{
		b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
	}
	double end_time = omp_get_wtime();
	printf("Dynamic: = %f \n", end_time - start_time);

	start_time = omp_get_wtime();
	#pragma omp parallel for schedule(guided, chunk_size)
	for (int i = 1; i < 15999; i++)
	{
		b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
	}
	end_time = omp_get_wtime();
	printf("Guided: = %f \n", end_time - start_time);

	start_time = omp_get_wtime();
	#pragma omp parallel for schedule(runtime)
	for (int i = 1; i < 15999; i++)
	{
		b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
	}
	end_time = omp_get_wtime();
	printf("Runtime: = %f \n", end_time - start_time);

	start_time = omp_get_wtime();
	#pragma omp parallel for schedule(static, chunk_size)
	for (int i = 1; i < 15999; i++)
	{
		b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
	}
	end_time = omp_get_wtime();
	printf("Static: = %f \n", end_time - start_time);
}

void ninth()
{
	int size = 3000;
	int chunk_size = 100;
	int* vector = new int[size];
	int** matrix = new int*[size];
	for (int i = 0; i < size; i++)
	{
		matrix[i] = new int[size];
		vector[i] = rand() % 10;
		for (int j = 0; j < size; j++)
		{
			matrix[i][j] = rand() % 10;
		}
	}
	double start_time = omp_get_wtime();
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			vector[i] = matrix[i][j] * vector[j];
		}
	}
	double end_time = omp_get_wtime();
	printf("Time for non-parallel: %f \n", end_time - start_time);
	start_time = omp_get_wtime();
	#pragma omp parallel for schedule(dynamic, chunk_size)
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			vector[i] = matrix[i][j] * vector[j];
		}
	}
	end_time = omp_get_wtime();
	printf("Time for parallel: %f \n", end_time - start_time);
}


/*
int main()
{
	sixth();
}
*/

