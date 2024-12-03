#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 1024
#define K 1024
#define N 1024


float gen_random_float(float min, float max)
{
	return ((float) rand() / RAND_MAX) * (max-min) + min;

}

int main()
{
	float* A, *B, *C;

	// memory allocation 
	if(!(A = (float*)malloc(sizeof(float) * M * K))){
		printf("Failed to malloc \n");
		return 0;
	}
	if(!(B = (float*)malloc(sizeof(float) * K * N))){
		printf("Failed to malloc \n");
		return 0;
	}
	if(!(C = (float*)malloc(sizeof(float) * M * N))){
		printf("Failed to malloc \n");
		return 0;
	}

	srand((unsigned int) time (NULL));

	// initialize 
	for (size_t rows = 0; rows < M; rows++) {
		for (size_t cols = 0; cols < K; cols++) {
			A[rows * K + cols] =  gen_random_float(0.1f, 0.0f);
			//printf("%lf ", A[rows*K+cols]);
		}
		//printf("\n");
	}

	for (size_t rows = 0; rows < K; rows++) {
		for (size_t cols = 0; cols < N; cols++) {
			B[rows * N + cols] =  gen_random_float(0.1f, 0.0f);
			//printf("%lf ", B[rows * M + cols]);
		}
		//printf("\n");
	}


	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			float c = 0.0;
			for (size_t k = 0; k < K; k++) {
				c += A[i * K + k] * B[k * N + j];
			}
			C[i * N + j] = c;
		}
	}

	printf("%lf\n", C[0]);

	free(A);
	free(B);
	free(C);


	return 0;
}
