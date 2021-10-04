#include <iostream>
#include <omp.h>



void first_task() {
#pragma omp parallel num_threads(8)
	{
		int count = omp_get_num_threads();
		int current_thread_num = omp_get_thread_num();
		printf("Hello world! current - %d, all - %d \n", current_thread_num, count);
	}
}

int main()
{
	first_task();
	return EXIT_SUCCESS;
}