#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "labyrinth.h"

void initialize_game(Dimensions dim, int game[dim.height][dim.width], PathPoints points) {
    int cpt = 0;

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (i == points.start_line && j == points.start_col) {
                game[i][j] = 'o'; 
            } else if (i == points.end_line && j == points.end_col) {
                game[i][j] = '-'; 
            } else if (i % 2 == 1 && j % 2 == 1 && i < dim.height - 1 && j < dim.width - 1) {
                if (cpt < 10) {
                    game[i][j] = '0' + cpt; 
                } else {
                    game[i][j] = 'A' + (cpt - 10); 
                }
                cpt++;
            }
            else {
                game[i][j] = '#'; 
            }
        }
    }
}

void upper_neighbor(Dimensions dim, int game[dim.height][dim.width], PathPoints *points) {
    points->start_line--;
    game[points->start_line][points->start_col] = (int)' ';
}

void lower_neighbor(Dimensions dim, int game[dim.height][dim.width], PathPoints *points) {
    points->start_line++;
    game[points->start_line][points->start_col] = (int)' ';
}

void left_neighbor(Dimensions dim, int game[dim.height][dim.width], PathPoints *points) {
    points->start_col--;
    game[points->start_line][points->start_col] = (int)' ';
}

void right_neighbor(Dimensions dim, int game[dim.height][dim.width], PathPoints *points) {
    points->start_col++;
    game[points->start_line][points->start_col] = (int)' ';
}

int count_found_cases(Dimensions dim, int game[dim.height][dim.width]) {
    int count = 0;
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (game[i][j] != '#' && game[i][j] != ' ' && 
                game[i][j] != 'o' && game[i][j] != '-') {
                count++;
            }
        }
    }
    return count;
}

void random_selection_case(Dimensions dim, int game[dim.height][dim.width]) {
    int count = count_found_cases(dim, game);

    
    int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right
    

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) 
        {
            if(count < 10){
                if(game[i][j] == count){
                    switch (direction) {
                        case 0: // up
                            if (i > 0 ) {
                                game[i - 1][j] = (int)' ';                      
                             }
                            break;
                        case 1: // down
                            if (i < dim.height - 1 ) {
                                game[i + 1][j] = (int)' ';                      
                             }
                            break;
                        case 2: // left
                            if (j > 0) {
                                game[i][j - 1] = (int)' ';                      
                             }
                            
                            break;
                        case 3: // right
                            if (j < dim.width - 1) {
                                game[i][j + 1] = (int)' ';                      
                             }
                            break;
                    }
                }
            }
        }
    }
}
        

void find_path(Dimensions dim, int game[dim.height][dim.width], PathPoints *points) {
    int **l = allocate_matrix(dim, -1);
    if (l == NULL) return;

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (game[i][j] != '#' && game[i][j] != ' ' && 
                game[i][j] != 'o' && game[i][j] != '-') {
                l[i][j] = game[i][j];
            } else {
                l[i][j] = -1;      
            }
        }
    }

    display_found_case(dim, l);
    free_matrix(dim, l);
}

void create_labyrinth(Dimensions dim) {
    srand(time(NULL)); 
    int game[dim.height][dim.width];

    PathPoints points;
    points.start_col = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    points.end_col   = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    points.start_line = 0;
    points.end_line = dim.height - 1;

    initialize_game(dim, game, points);
    find_path(dim, game, &points);
    display_labyrinth(dim, game);
}
