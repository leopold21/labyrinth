#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

typedef struct labyrinth
{
    Dimensions dim;
    int **board;
    PathPoints points;
} Labyrinth;


// Prototypes of the interface 
void menu(void);
Dimensions set_dimensions(void);
void set_name(char *name);
void display_labyrinth(Dimensions dim, int **labyrinth);
void display_found_case(Dimensions dim, int **l);

// Prototypes of the memory management functions
int *allocate_vector(int dimension, int val);
int **allocate_matrix(Dimensions dim, int val);
void free_matrix(Dimensions dim, int **matrix);

// Prototypes Labyrinth functions
void initialize_game(Labyrinth labyrinth, PathPoints points);
int **allocate_labyrinth(Dimensions dim);
void free_labyrinth(Labyrinth labyrinth);

bool is_walkable_cell(Point point, Labyrinth labyrinth);
bool is_upper_cell_walkable(Point point, Labyrinth labyrinth);
bool is_lower_cell_walkable(Point point, Labyrinth labyrinth);
bool is_left_cell_walkable(Point point, Labyrinth labyrinth);
bool is_right_cell_walkable(Point point, Labyrinth labyrinth);

Point upper_neighbor(Point point, Dimensions dim);
Point lower_neighbor(Point point, Dimensions dim);
Point left_neighbor(Point point, Dimensions dim);
Point right_neighbor(Point point, Dimensions dim);

Point random_selection_case(Labyrinth labyrinth);
void merge_components(Labyrinth labyrinth, Point p1, Point p2, Point p3);
bool is_generate_path(Labyrinth labyrinth);
void generate_path(Labyrinth  labyrinth);

Labyrinth* create_labyrinth(Dimensions dim);
void save_labyrinth(const char *fileName, const char *userName, Labyrinth *labyrinth);
void clear_buffer();
#endif