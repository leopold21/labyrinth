#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "labyrinth.h"

void initialize_game(Dimensions dim, int game[dim.height][dim.width], PathPoints points) {
    int cpt = 0;

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (i == points.start_line && j == points.start_col) {
                game[i][j] = START;
            } else if (i == points.end_line && j == points.end_col) {
                game[i][j] = END;
            } else if (i % 2 == 1 && j % 2 == 1 && i < dim.height - 1 && j < dim.width - 1) {
                game[i][j] = cpt;
                cpt++;
            }
            else {
                game[i][j] = WALL; 
            }
        }
        printf("\n");
    }
}

/*
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
*/

int count_found_cases(Dimensions dim, int game[dim.height][dim.width]) {
    int count = 0;
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (game[i][j] != WALL && 
                game[i][j] != START && 
                game[i][j] != END) {
                count++;
            }
        }
    }
    return count;
}

Point random_selection_case(Dimensions dim, int game[dim.height][dim.width]) {
    Point wall = {-1, -1};
    int i, j;

    while (1) {
        i = 1 + rand() % (dim.height - 2); 

        if (i % 2 == 1) {
            j = 2 + 2 * (rand() % ((dim.width - 2) / 2));
        }
        else {
            j = 1 + 2 * (rand() % ((dim.width - 1) / 2));
        }

        if (game[i][j] != WALL) {
            continue;
        }

        
        if (i % 2 == 1) { 
            if (j - 1 >= 0 && j + 1 < dim.width) {
                if (game[i][j - 1] != game[i][j + 1]) {
                    break; 
                }
            }
        } 
        else { 
            if (i - 1 >= 0 && i + 1 < dim.height) {
                if (game[i - 1][j] != game[i + 1][j]) {
                    break; 
                }
            }
        }
    }

    wall.line = i;
    wall.col  = j;
    return wall;
}
    /*
    int direction = rand() % 4; // 0: up, 1: down, 2: left, 3: right
    printf("Random direction: %d\n", direction); 

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) 
        {
            if(count < 10){
                if(game[i][j] == count){
                    switch (direction) {
                        case 0: // up
                            printf("On monte, on a comme futur coordonnées: (%d,%d)\n", i - 1, j);
                            if (i > 0 ) {
                                game[i - 1][j] = (int)' ';                      
                             }
                            break;
                        case 1: // down
                            printf("On monte, on a comme futur coordonnées: (%d,%d)\n", i + 1, j);
                            if (i < dim.height - 1 ) {
                                game[i + 1][j] = (int)' ';                      
                             }
                            break;
                        case 2: // left
                            if (j > 0) {
                                printf("On monte, on a comme futur coordonnées: (%d,%d)\n", i, j - 1);
                                game[i][j - 1] = (int)' ';                      
                             }
                            
                            break;
                        case 3: // right
                            if (j < dim.width - 1) {
                                printf("On monte, on a comme futur coordonnées: (%d,%d)\n", i, j + 1);
                                game[i][j + 1] = (int)' ';                      
                             }
                            break;
                    }
                }
            }
        }
    }
    */
        

void generate_path(Dimensions dim, int game[dim.height][dim.width]) {
    int count = count_found_cases(dim, game);
    count = 2; // test
    Point wall;
    while (count>1) {
        printf("%d\n", count);
        wall = random_selection_case(dim, game); // renvoie mur cassable
        printf("line: %d, col: %d\n", wall.line, wall.col);
         game[wall.line][wall.col] = ' '; 
        // gerer le wa ou wall n'est pas défini ?
        // casse le mur et fusionne les pieces
        count--;
    }
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
   generate_path(dim, game);
    display_labyrinth(dim, game);
}
