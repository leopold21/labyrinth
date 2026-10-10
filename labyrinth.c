#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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
            } else {
                game[i][j] = WALL; 
            }
        }
    }
}


bool is_walkable_cell(Point point, Dimensions dim, int game[dim.height][dim.width]) {
    if (point.line < 0 || point.line >= dim.height || point.col < 0 || point.col >= dim.width) {
        return false;
    }
    int val_point = game[point.line][point.col];
    return (val_point != WALL && val_point != START && val_point != END);
}

bool is_upper_cell_walkable(Point point, Dimensions dim, int game[dim.height][dim.width]) {
    Point upper_point = {point.line - 1, point.col};
    return is_walkable_cell(upper_point, dim, game);
}

bool is_lower_cell_walkable(Point point, Dimensions dim, int game[dim.height][dim.width]) {
    Point lower_point = {point.line + 1, point.col};
    return is_walkable_cell(lower_point, dim, game);
}

bool is_left_cell_walkable(Point point, Dimensions dim, int game[dim.height][dim.width]) {
    Point left_point = {point.line, point.col - 1};
    return is_walkable_cell(left_point, dim, game);
}

bool is_right_cell_walkable(Point point, Dimensions dim, int game[dim.height][dim.width]) {
    Point right_point = {point.line, point.col + 1};
    return is_walkable_cell(right_point, dim, game);
}

Point upper_neighbor(Point point, Dimensions dim) {
    Point upper_point = {point.line - 1, point.col};
    return upper_point;
}

Point lower_neighbor(Point point, Dimensions dim) {
    Point lower_point = {point.line + 1, point.col};
    return lower_point;
}

Point left_neighbor(Point point, Dimensions dim) {
    Point left_point = {point.line, point.col - 1};
    return left_point;
}

Point right_neighbor(Point point, Dimensions dim) {
    Point right_point = {point.line, point.col + 1};
    return right_point;
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

void merge_components(Dimensions dim, int game[dim.height][dim.width], Point p1, Point p2, Point p3) {
    int old_p3 = game[p3.line][p3.col];
    game[p2.line][p2.col] = game[p1.line][p1.col];
    //game[p3.line][p3.col] = game[p2.line][p2.col]; // pourquoi ça ne fonctionne pas pour p3, on est obligé de faire la boucle

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (game[i][j] == old_p3) {
                game[i][j] = game[p2.line][p2.col];
            }
        }
    }

}
    


void generate_path(Dimensions dim, int game[dim.height][dim.width]) {
    Point wall;
    int walls_opened = 0;
    Point p1, p2, p3;

    while (walls_opened < (((dim.height - 1) / 2) * ((dim.width - 1) / 2)) -1) { //voir pourquoi cette formule
        wall = random_selection_case(dim, game);
        if (wall.line == -1 && wall.col == -1) {
            break; 
        }
        
        if(wall.line%2 == 0){
            p1.line = wall.line - 1;
            p1.col = wall.col;
            
            p2.line = wall.line;
            p2.col = wall.col;

            p3.line = wall.line + 1;
            p3.col = wall.col;
            merge_components(dim, game, p1, p2, p3);
        }
        else{
            p1.line = wall.line;
            p1.col = wall.col - 1;
            
            p2.line = wall.line;
            p2.col = wall.col;

            p3.line = wall.line;
            p3.col = wall.col + 1;            
            merge_components(dim, game, p1, p2, p3);
        }

        walls_opened++;
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