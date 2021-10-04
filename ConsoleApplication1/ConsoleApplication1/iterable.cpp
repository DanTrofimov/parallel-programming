#include <iostream>
#include <omp.h>


void first() {
#pragma omp parallel num_threads(8)
	{
		int count = omp_get_num_threads();
		int current_thread_id = omp_get_thread_num();
		printf("Hello world! Current thread - %d, threads amount - %d \n", current_thread_id, count);
	}
}

void second() {
	const int comparator = 2;
	int threads_amount = 3;
#pragma omp parallel num_threads(threads_amount) if(threads_amount > comparator)
	{
		printf("First parallel area, current thread: %d, available threads: %d \n", omp_get_thread_num(), omp_get_num_threads());
	}
	threads_amount = 2;
#pragma omp parallel num_threads(threads_amount) if (threads_amount > comparator)
	{
		printf("Second parallel area, current thread %d, available threads %d \n", omp_get_thread_num(), omp_get_num_threads());
	}
}

void third() {
	int a = 0;
	int b = 0;
	printf("Variables before first parrallel block: a: %d, b: %d \n", a, b);
#pragma omp parallel num_threads(2) private(a) firstprivate(b)
	{
		int current_thread_id = omp_get_thread_num();
		a = 0;
		a += current_thread_id;
		b += current_thread_id;
		printf("Current thread - %d, a - %d, b - %d \n", current_thread_id, a, b);
	}
	printf("Variables after first parrallel block: a: %d, b: %d \n", a, b);
	printf("Variables before second parrallel block a: %d, b: %d \n", a, b);
#pragma omp parallel num_threads(4) shared(a) private(b)
	{
		b = 4;
		int current_thread_num = omp_get_thread_num();
		a -= current_thread_num;
		b -= current_thread_num;
		printf("current_thread_num - %d, a - %d, b - %d \n", current_thread_num, a, b);
	}
	printf("after second block a - %d, b - %d \n", a, b);
}

void fouth()
{
	int a[10];
	int b[10];
	for (int i = 0; i < 10; i++)
	{
		a[i] = rand() % 10;
		b[i] = rand() % 10;
	}
#pragma omp parallel num_threads(2)
	{
#pragma omp master
		{
			int min = 10;
			for (int i = 0; i < 10; i++)
			{
				if (a[i] < min) min = a[i];
			}
			printf("Minimum value: %d\n", min);
		}
#pragma omp single 
		{
			int max = -1;
			for (int i = 0; i < 10; i++)
			{
				if (b[i] > max) max = a[i];
			}
			printf("Maximum value: %d", max);
		}
	}
}

void fifth()
{
	int d[6][8];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 8; j++) {
			d[i][j] = rand() % 10;
		}
	}
#pragma omp parallel sections num_threads(3)
	{
#pragma omp section 
		{
			int sum = 0;
			int items_amount = 6 * 8;
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 8; j++) {
					sum += d[i][j];
				}
			}
			printf("Average value: %d, current thread: %d \n", sum / items_amount, omp_get_thread_num());
		}
#pragma omp section 
		{
			int min = 10;
			int max = -1;
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 8; j++) {
					if (d[i][j] > max) max = d[i][j];
					if (d[i][j] < min) min = d[i][j];
				}
			}
			printf("Minimum value: %d, maximum value: %d, current thread: %d \n", min, max, omp_get_thread_num());
		}
#pragma omp section 
		{
			int count = 0;
			for (int i = 0; i < 6; i++) {
				for (int j = 0; j < 8; j++) {
					if (d[i][j] % 3 == 0) count++;
				}
			}
			printf("Num % 3 == 0 amount: %d, current thread; %d \n", count, omp_get_thread_num());
		}
	}
}

int main()
{
	fifth();
	return EXIT_SUCCESS;
}
