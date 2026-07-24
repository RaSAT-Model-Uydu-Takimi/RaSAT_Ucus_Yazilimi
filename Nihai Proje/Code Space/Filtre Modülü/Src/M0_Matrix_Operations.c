#include "M0_Matrix_Operations.h"
#include <math.h>

void mat_add(int rows, int cols, const float *A, const float *B, float *C) {
    int size = rows * cols;
    for (int i = 0; i < size; i++) {
        C[i] = A[i] + B[i];
    }
}

void mat_sub(int rows, int cols, const float *A, const float *B, float *C) {
    int size = rows * cols;
    for (int i = 0; i < size; i++) {
        C[i] = A[i] - B[i];
    }
}

void mat_mult(int rowsA, int colsA, int colsB, const float *A, const float *B, float *C) {
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            float sum = 0.0f;
            for (int k = 0; k < colsA; k++) {
                sum += A[i * colsA + k] * B[k * colsB + j];
            }
            C[i * colsB + j] = sum;
        }
    }
}

void mat_trans(int rows, int cols, const float *A, float *C) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            C[j * rows + i] = A[i * cols + j];
        }
    }
}

bool mat_inv(int n, const float *A, float *C) {
    if (n > MAX_MATRIX_SIZE) return false;

    float mat[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    float inv[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mat[i][j] = A[i * n + j];
            if (i == j) inv[i][j] = 1.0f;
            else inv[i][j] = 0.0f;
        }
    }

    for (int i = 0; i < n; i++) {
        if (fabs(mat[i][i]) < 1e-6f) {
            int swapRow = -1;
            for (int k = i + 1; k < n; k++) {
                if (fabs(mat[k][i]) > 1e-6f) {
                    swapRow = k;
                    break;
                }
            }
            if (swapRow == -1) return false;

            for (int j = 0; j < n; j++) {
                float temp = mat[i][j];
                mat[i][j] = mat[swapRow][j];
                mat[swapRow][j] = temp;

                temp = inv[i][j];
                inv[i][j] = inv[swapRow][j];
                inv[swapRow][j] = temp;
            }
        }

        float pivot = mat[i][i];
        if (fabs(pivot) < 1e-6f) return false;
        
        for (int j = 0; j < n; j++) {
            mat[i][j] /= pivot;
            inv[i][j] /= pivot;
        }

        for (int k = 0; k < n; k++) {
            if (k != i) {
                float factor = mat[k][i];
                for (int j = 0; j < n; j++) {
                    mat[k][j] -= factor * mat[i][j];
                    inv[k][j] -= factor * inv[i][j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i * n + j] = inv[i][j];
        }
    }

    return true;
}
