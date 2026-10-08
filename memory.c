#include <stdio.h>
#include <stdlib.h>

// Structure pour la dimension du labyrinthe
typedef struct {
    int height;
    int width;
} Dimensions;

// Structure pour les entrées et sorties du labyrinthe
typedef struct {
    int start_line;
    int start_col;
    int end_line;
    int end_col;
} PathPoints;

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
        if (matrix[i] == NULL) return NULL;
    }

    return matrix;
}

void free_matrix(Dimensions dim, int **matrix) {
    if (matrix == NULL) return;
    for (int i = 0; i < dim.height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

