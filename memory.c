#include <stdio.h>
#include <stdlib.h>

#include "labyrinth.h"

/**
 * @brief Allocates a 1D integer array and initializes all elements with a default value.
 * 
 * @param dimension Number of integer elements to allocate.
 * @param val Initial value used to fill each element in the vector.
 * 
 * @return int* Pointer to the dynamically allocated array, or NULL if memory allocation fails.
 */
int *allocate_vector(int dimension, int val) {
    int *vector = malloc(sizeof(int) * dimension);
    if (vector == NULL) return NULL;
    for (int i = 0; i < dimension; i++) {
        vector[i] = val;
    }
    return vector;
}

/**
 * @brief Allocates a 2D integer matrix with specified dimensions and initializes all cells.
 *  
 * @param dim Dimensions structure defining matrix height (rows) and width (columns).
 * @param val Initial value used to fill each cell in the 2D matrix.
 * 
 * @return int** Pointer to the array of row pointers, or NULL if allocation fails.
 */
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

/**
 * @brief Clears remaining unread characters from standard input (`stdin`).
 * 
 * @return void
 */

void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
}

/**
 * @brief Deallocates memory assigned to a 2D integer matrix and its row pointers.
 * 
 * @param dim Dimensions structure defining the height of the matrix.
 * @param matrix Pointer to the 2D matrix array to free. Safe to call if NULL.
 * 
 * @return void
 */
void free_matrix(Dimensions dim, int **matrix) {
    if (matrix == NULL) return;
    for (int i = 0; i < dim.height; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
