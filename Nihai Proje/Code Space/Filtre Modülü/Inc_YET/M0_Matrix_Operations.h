#ifndef INC_M0_MATRIX_OPERATIONS_H_
#define INC_M0_MATRIX_OPERATIONS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Başlangıç
#define MAX_MATRIX_SIZE 4

void mat_add(int rows, int cols, const float *A, const float *B, float *C);
void mat_sub(int rows, int cols, const float *A, const float *B, float *C);
void mat_mult(int rowsA, int colsA, int colsB, const float *A, const float *B, float *C);
void mat_trans(int rows, int cols, const float *A, float *C);
bool mat_inv(int n, const float *A, float *C);

// Bitiş

#ifdef __cplusplus
}
#endif

#endif /* INC_M0_MATRIX_OPERATIONS_H_ */
