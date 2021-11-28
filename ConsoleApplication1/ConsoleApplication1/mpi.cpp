#include <iostream>
#include <mpi.h>

// current thread, threads amount
int rank, size;
const int array_size = 5;

// Hello world из всех процессов 
void task1() {
	printf("Hello world, size = %d, rank = %d \n", size, rank);
}

// Maксимум массива  
void task2() {
	int array[array_size];
	if (rank == 0) {
		for (int i = 0; i < array_size; i++) {
			array[i] = rand();
		}
	}
	int offset = array_size / size;
	int start = rank * offset;
	int end = offset * (rank + 1);
	if (rank == size - 1) {
		end = size;
	}
	MPI_Bcast(&array, array_size, MPI_INT, 0, MPI_COMM_WORLD);
	int max = array[start];
	for (int i = start + 1; i < end; i++) {
		if (array[i] > max) max = array[i];
	}
	int abs_max;
	MPI_Reduce(&max, &abs_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	printf("local_max = %d, rank = %d \n", max, rank);
	if (rank == 0) {
		printf("max = %d \n", abs_max);
	}
}

// Вычисление числа Пи методом Монте-Карло
void task3() {
	int radius = 100;
	int square_side = radius * 2;
	int point_in_circle_one_rank = 0;
	int iteration_for_one_rank = 1e7;
	for (int i = 0; i < iteration_for_one_rank; i++) {
		int x = rand() % square_side - square_side / 2;
		int y = rand() % square_side - square_side / 2;
		if ((x * x + y * y) <= radius * radius)
			point_in_circle_one_rank++;
	}
	int all_points_in_circle = 0;
	MPI_Reduce(&point_in_circle_one_rank, &all_points_in_circle, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		double pi = 4.0 * all_points_in_circle / ((double)iteration_for_one_rank * size);
		printf("PI = %.10f \n", pi);
	}
}

// Среднее арифметическое среди положительных чисел массива 
void task4() {
	int offset = array_size / size;
	int buffer_size = offset;
	int* buffer = new int[buffer_size];
	int* scounts = new int[size];
	int* displs = new int[size];
	int array[array_size];
	for (int i = 0; i < size - 1; i++) {
		scounts[i] = offset;
		displs[i] = i * offset;
	}
	scounts[size - 1] = offset + array_size % size;
	displs[size - 1] = (size - 1) * array_size / size;
	if (rank == 0) {
		for (int i = 0; i < array_size; i++) {
			array[i] = rand() % 100;
		}
	}
	for (int i = 0; i < size; i++) {
		if (rank == i) {
			buffer = new int[scounts[i]];
			break;
		}
	}
	MPI_Scatterv(array, scounts, displs, MPI_INT, buffer, buffer_size, MPI_INT, 0, MPI_COMM_WORLD);
	int local_count = 0;
	int local_sum = 0;
	for (int i = 0; i < buffer_size; i++) {
		if (buffer[i] > 0) {
			local_sum += buffer[i];
			local_count++;
		}
	}
	int time_buffer[2] = { local_sum, local_count };
	const int new_buf_size = 2 * size;
	int* new_buffer = new int[new_buf_size];
	MPI_Gather(&time_buffer[0], 2, MPI_INT, &new_buffer[0], 2, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		int sum = 0;
		int count = 0;
		for (int i = 0; i < new_buf_size; i++) {
			if (i % 2 == 0) {
				sum += new_buffer[i];
			}
			else {
				count += new_buffer[i];
			}
		}
		printf("Average value = %d \n", sum / count);
	}
}

// Scatter и Gather через Send и Recv
void task8() {
	int buffer_size = array_size / size;
	int* buffer = new int[array_size / size];
	MPI_Status status;
	int* array = new int[array_size];
	int* new_array = new int[array_size];
	if (rank == 0) {
		for (int i = 0; i < array_size; i++) {
			array[i] = rand();
		}
		for (int i = 0; i < array_size; i++) {
			printf("a[%d] = %d \n ", i, array[i]);
		}
		for (int i = 1; i < size; i++) {
			MPI_Send(&array[i * buffer_size], buffer_size, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		for (int i = 0; i < array_size / size; i++) {
			buffer[i] = array[i];
			new_array[i] = buffer[i];
		}
	}
	else {
		MPI_Recv(buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	for (int i = 0; i < array_size / size; i++) {
		printf("b[%d] = %d, rank = %d \n ", i, buffer[i], rank);
	}
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			MPI_Recv(buffer, buffer_size, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
			int start_index = i * array_size / size;
			for (int i = start_index; i < start_index + sizeof(buffer); i++) {
				new_array[i] = buffer[i - start_index];
			}
		}
		for (int i = 0; i < array_size; i++) {
			printf("c[%d] = %d \n ", i, new_array[i]);
		}
	}
	else {
		MPI_Send(buffer, buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
}

// Инвертировать массив
void task9() {
	int buffer_size = array_size / size;
	int* buffer = new int[buffer_size];
	MPI_Status status;
	if (rank == 0) {
		int* array = new int[array_size];
		for (int i = 0; i < array_size; i++) {
			array[i] = rand();
		}
		for (int i = 0; i < array_size; i++) {
			printf("a[%d] = %d \n", i, array[i]);
		}
		for (int i = 1; i < size; i++) {
			MPI_Send(&array[i * buffer_size], buffer_size, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		for (int i = 0; i < buffer_size; i++) {
			buffer[i] = array[i];
		}
	}
	else {
		MPI_Recv(buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	}
	for (int i = 0; i < buffer_size / 2; i++) {
		int c = buffer[i];
		buffer[i] = buffer[buffer_size - i - 1];
		buffer[buffer_size - i - 1] = c;
	}
	if (rank == 0) {
		int* new_array = new int[array_size];
		for (int i = 1; i < size; i++) {
			MPI_Recv(&new_array[array_size - (i + 1) * buffer_size], buffer_size, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
		}
		int start_index = array_size - buffer_size;
		for (int i = 0; i < buffer_size; i++) {
			new_array[start_index + i] = buffer[i];
		}
		for (int i = 0; i < array_size; i++) {
			printf("b[%d] = %d \n", i, new_array[i]);
		}
	}
	else {
		MPI_Send(buffer, buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
}

// Циклическая передача данных 
void task11() {
	MPI_Status st;
	int value;
	if (rank == 0) {
		value = 1;
		MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&value, 1, MPI_INT, size - 1, size - 1, MPI_COMM_WORLD, &st);
		printf("value = %d, rank = %d \n", value, rank);
	}
	else {
		int sender = rank - 1;
		int reciver = rank + 1;
		if (rank == size - 1) reciver = 0;
		MPI_Recv(&value, 1, MPI_INT, sender, sender, MPI_COMM_WORLD, &st);
		printf("value = %d, rank = %d \n", value, rank);
		value += rank;
		MPI_Send(&value, 1, MPI_INT, reciver, rank, MPI_COMM_WORLD);
	}
}


// --- new ---

// Скалярное произведение
void task5() {
	const int matrix_size = array_size * array_size;
	int offset = matrix_size / size;
	// amount of ops for each rank
	int* scounts = new int[size];
	// offset from the beginning of the transmit buffer
	int* displs = new int[size];
	for (int i = 0; i < size - 1; i++) {
		scounts[i] = offset;
		displs[i] = i * offset;
	}
	scounts[size - 1] = offset + matrix_size % size;
	displs[size - 1] = (size - 1) * matrix_size % size;

	int* matrixA = new int[matrix_size];
	int* matrixB = new int[matrix_size];
	// buffer for each process
	int* buffer_matrixA = new int[matrix_size / size];
	int* buffer_matrixB = new int[matrix_size / size];

	long* buffer_c = new long[matrix_size / size];
	if (rank == 0) {
		// filling matrix
		matrixA = new int[matrix_size];
		matrixB = new int[matrix_size];
		for (int i = 0; i < array_size; i++) {
			for (int k = 0; k < array_size; k++) {
				matrixA[i * array_size + k] = rand() % 100;
				matrixB[k * array_size + i] = rand() % 100;
			}
		}
	}

	MPI_Scatterv(matrixA, scounts, displs, MPI_INT, buffer_matrixA, matrix_size / size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatterv(matrixB, scounts, displs, MPI_INT, buffer_matrixB, matrix_size / size, MPI_INT, 0, MPI_COMM_WORLD);
	for (int i = 0; i < array_size / size; i++) {
			for (int t = 0; t < array_size; t++) {
				buffer_c[i * array_size + t] = 0;
				int c_index = i * array_size + t;
				int a_index = i * array_size + t;
				int b_index = i * array_size + t;
				buffer_c[c_index] = buffer_matrixA[a_index] * buffer_matrixB[b_index];
			}
	}
	long* matrixC = new long[matrix_size];
	MPI_Gather(buffer_c, matrix_size / size, MPI_LONG, matrixC, matrix_size / size, MPI_LONG, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		for (int i = 0; i < array_size; i++) {
			for (int k = 0; k < array_size; k++) {
				printf("%d ", matrixA[i * array_size + k]);
			}
			printf(" \n");
		}

		printf(" \n");


		for (int i = 0; i < array_size; i++) {
			for (int k = 0; k < array_size; k++) {
				printf("%d ", matrixB[i * array_size + k]);
			}
			printf(" \n");
		}

		printf(" \n");
		
		for (int i = 0; i < array_size; i++) {
			for (int k = 0; k < array_size; k++) {
				printf("%d ", matrixC[i * array_size + k]);
			}
			printf(" \n");
		}
	}
}

// Maxmin матрицы
void task6() {
	const int matrix_size = array_size * array_size;
	const int offset = matrix_size / size;
	int buffer_size = offset;
	int* buffer = new int[buffer_size];
	// amount of ops for each rank
	int* scounts = new int[size];
	// offset from the beginning of the transmit buffer
	int* displs = new int[size];
	int* array = new int[matrix_size];
	for (int i = 0; i < size - 1; i++) {
		scounts[i] = offset;
		displs[i] = i * offset;
	}
	scounts[size - 1] = offset + array_size % size;
	displs[size - 1] = (size - 1) * array_size / size;

	// filling matrix
	if (rank == 0) {
		for (int i = 0; i < matrix_size; i++) {
			array[i] = rand() % 10;
		}
	}

	for (int i = 0; i < size; i++) {
		if (rank == i) {
			buffer = new int[scounts[i]];
			break;
		}
	}

	for (int i = 0; i < array_size; i++) {
		for (int k = 0; k < array_size; k++) {
			printf("%d ", array[i * array_size + k]);
		}
		printf(" \n");
	}

	MPI_Scatterv(array, scounts, displs, MPI_INT, buffer, buffer_size, MPI_INT, 0, MPI_COMM_WORLD);
	int maxmin = buffer[0];
	for (int i = 0; i < array_size / size; i++) {
		int min = buffer[i * array_size];
		int max = buffer[i * array_size];
		// finding max element from the min element from each column 
		for (int k = 0; k < array_size; k++) {
			if (min > buffer[i * array_size + k]) min = buffer[i * array_size + k];
		}
		if (maxmin < min) maxmin = min;
	}
	int min_max_buffer[1] = { maxmin };
	int* min_max = new int[size];
	MPI_Gather(min_max_buffer, 1, MPI_INT, min_max, 1, MPI_INT, 0, MPI_COMM_WORLD);
	maxmin = min_max[0];
	if (rank == 0) {
		for (int i = 0; i < size; i++) {
			if (maxmin < buffer[i]) maxmin = buffer[i];
		}
		printf("maxmin = %d \n", maxmin);
	}
}

// Умножение матрицы на вектор при разделении данных по столбцам
void task7() {
}

// Время передачи для разных Send-oв
void task10() {
}


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	task6();
	MPI_Finalize();
	return 0;
}	