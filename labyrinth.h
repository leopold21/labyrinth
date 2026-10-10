#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum cases {
    WALL = -1,
    START = -2,
    END = -3,
    KEY = -4
};

// Structures
typedef struct {
    int height;
    int width;
} Dimensions;

typedef struct {
    int start_line;
    int start_col;
    int end_line;
    int end_col;
} PathPoints;

typedef struct {
    int line;
    int col;
} Point;

// Prototypes of the interface 
void menu(void);
Dimensions set_dimensions(void);
void set_name(char *name);
void display_labyrinth(Dimensions dim, int game[dim.height][dim.width]);
void display_found_case(Dimensions dim, int **l);

// Prototypes of the memory management functions
int *allocate_vector(int dimension, int val);
int **allocate_matrix(Dimensions dim, int val);
void free_matrix(Dimensions dim, int **matrix);

// Prototypes Labyrinth functions
void initialize_game(Dimensions dim, int game[dim.height][dim.width], PathPoints points);
Point upper_neighbor(Point point, Dimensions dim);
Point lower_neighbor(Point point, Dimensions dim);
Point left_neighbor(Point point, Dimensions dim);
Point right_neighbor(Point point, Dimensions dim);
int count_found_cases(Dimensions dim, int game[dim.height][dim.width]);
Point random_selection_case(Dimensions dim, int game[dim.height][dim.width]);
void find_path(Dimensions dim, int game[dim.height][dim.width], PathPoints *points);
void create_labyrinth(Dimensions dim);

#endif