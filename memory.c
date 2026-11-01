#include <stdio.h>
#include <stdlib.h>

#include "labyrinth.h"

int *allocate_vector(int dimension, int val) {
    int *vector = malloc(sizeof(int) * dimension);
    if (vector == NULL) return NULL;
    for (int i = 0; i < dimension; i++) {
        vector[i] = val;
    }
    return vector;
}

int **allocate_matrix(Dimensions dim, int val) {
    int **matrix = malloc(sizeof(int *) * dim.height);
    if (matrix == NULL) return NULL;

    for (int i = 0; i < dim.height; i++) {
        matrix[i] = allocate_vector(dim.width, val);
        if (matrix[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
}
void free_matrix(Dimensions dim, int **matrix) {
    if (matrix == NULL) return;
    for (int i = 0; i < dim.height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
