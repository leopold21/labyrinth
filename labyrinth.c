#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "labyrinth.h"

void initialize_game(Labyrinth labyrinth, PathPoints points) {
    int cpt = 0;

    for (int i = 0; i < labyrinth.dim.height; i++) {
        for (int j = 0; j < labyrinth.dim.width; j++) {
            if (i == points.start_line && j == points.start_col) {
                labyrinth.board[i][j] = START;
            } else if (i == points.end_line && j == points.end_col) {
                labyrinth.board[i][j] = END;
            } else if (i % 2 == 1 && j % 2 == 1 &&
                       i < labyrinth.dim.height - 1 && j < labyrinth.dim.width - 1) {
                labyrinth.board[i][j] = cpt;
                cpt++;
            } else {
                labyrinth.board[i][j] = WALL;
            }
        }
    }
}

int **allocate_labyrinth(Dimensions dim) {
    int **labyrinth = malloc(dim.height * sizeof(int *));
    if (labyrinth == NULL) {
        perror("Erreur d'allocation mémoire pour les lignes");
        return NULL;
    }
    for (int i = 0; i < dim.height; i++) {
        labyrinth[i] = malloc(dim.width * sizeof(int));
        if (labyrinth[i] == NULL) {
            perror("Erreur d'allocation mémoire pour les colonnes");
            while (i > 0) {
                free(labyrinth[--i]);
            }
            free(labyrinth);
            return NULL;
        }
    }
    return labyrinth;
}

void free_labyrinth(Labyrinth labyrinth) {
    if (labyrinth.board == NULL) return;
    for (int i = 0; i < labyrinth.dim.height; i++) {
        free(labyrinth.board[i]);
    }
    free(labyrinth.board);
}

bool is_walkable_cell(Point point, Labyrinth labyrinth) {
    if (point.line < 0 || point.line >= labyrinth.dim.height ||
        point.col < 0 || point.col >= labyrinth.dim.width) {
        return false;
    }
    int val_point = labyrinth.board[point.line][point.col];
    return (val_point != WALL && val_point != START && val_point != END);
}

bool is_upper_cell_walkable(Point point, Labyrinth labyrinth) {
    Point upper_point = {point.line - 1, point.col};
    return is_walkable_cell(upper_point, labyrinth);
}

bool is_lower_cell_walkable(Point point, Labyrinth labyrinth) {
    Point lower_point = {point.line + 1, point.col};
    return is_walkable_cell(lower_point, labyrinth);
}

bool is_left_cell_walkable(Point point, Labyrinth labyrinth) {
    Point left_point = {point.line, point.col - 1};
    return is_walkable_cell(left_point, labyrinth);
}

bool is_right_cell_walkable(Point point, Labyrinth labyrinth) {
    Point right_point = {point.line, point.col + 1};
    return is_walkable_cell(right_point, labyrinth);
}

Point upper_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point upper_point = {point.line - 1, point.col};
    return upper_point;
}

Point lower_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point lower_point = {point.line + 1, point.col};
    return lower_point;
}

Point left_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point left_point = {point.line, point.col - 1};
    return left_point;
}

Point right_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point right_point = {point.line, point.col + 1};
    return right_point;
}

Point random_selection_case(Labyrinth labyrinth) {
    Dimensions dim = labyrinth.dim;
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

        if (labyrinth.board[i][j] != WALL) {
            continue;
        }

        if (i % 2 == 1) { 
            if (j - 1 >= 0 && j + 1 < dim.width) {
                if (labyrinth.board[i][j - 1] != labyrinth.board[i][j + 1]) {
                    break; 
                }
            }
        } 
        else { 
            if (i - 1 >= 0 && i + 1 < dim.height) {
                if (labyrinth.board[i - 1][j] != labyrinth.board[i + 1][j]) {
                    break; 
                }
            }
        }
    }
    wall.line = i;
    wall.col  = j;
    return wall;
}

void merge_components(Labyrinth labyrinth, Point p1, Point p2, Point p3) {
    int old_p3 = labyrinth.board[p3.line][p3.col];
    labyrinth.board[p2.line][p2.col] = labyrinth.board[p1.line][p1.col];

    for (int i = 0; i < labyrinth.dim.height; i++) {
        for (int j = 0; j < labyrinth.dim.width; j++) {
            if (labyrinth.board[i][j] == old_p3) {
                labyrinth.board[i][j] = labyrinth.board[p2.line][p2.col];
            }
        }
    }
}

bool is_generate_path(Labyrinth labyrinth) {
    Point point;
    int number = -1;
    for (int i = 0; i < labyrinth.dim.height; i++) {
        for (int j = 0; j < labyrinth.dim.width; j++) {
            point.line = i;
            point.col = j;
            if (is_walkable_cell(point, labyrinth)) {
                if (number == -1) {
                    number = labyrinth.board[i][j];
                }
                else if (labyrinth.board[i][j] != number) {
                    return false;
                }
            }
        }
    }
    return true;
}

void generate_path(Labyrinth labyrinth) {
    Point wall;
    Point p1, p2, p3;

    while (!is_generate_path(labyrinth)) {
        wall = random_selection_case(labyrinth);
        if (wall.line == -1 && wall.col == -1) {
            break; 
        }
        
        if (wall.line % 2 == 0) {
            p1.line = wall.line - 1;
            p1.col = wall.col;
            
            p2.line = wall.line;
            p2.col = wall.col;

            p3.line = wall.line + 1;
            p3.col = wall.col;
            merge_components(labyrinth, p1, p2, p3);
        }
        else {
            p1.line = wall.line;
            p1.col = wall.col - 1;
            
            p2.line = wall.line;
            p2.col = wall.col;

            p3.line = wall.line;
            p3.col = wall.col + 1;            
            merge_components(labyrinth, p1, p2, p3);
        }
    }
}

Labyrinth* create_labyrinth(Dimensions dim) {
    srand(time(NULL)); 

    Labyrinth *labyrinth = malloc(sizeof(Labyrinth));
    if (labyrinth == NULL) {
        perror("Erreur d'allocation mémoire pour le labyrinthe");
        return NULL;
    }

    labyrinth->dim = dim;
    labyrinth->board = allocate_labyrinth(dim);
    if (labyrinth->board == NULL) {
        free(labyrinth);
        return NULL;
    }

    PathPoints points;
    points.start_col  = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    points.end_col    = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    points.start_line = 0;
    points.end_line   = dim.height - 1;
    labyrinth->points = points;

    initialize_game(*labyrinth, points);
    generate_path(*labyrinth);

    return labyrinth;
}

void create_directory(char *path) {
    char *sep = strrchr(path, '/');
    if(sep != NULL) {
        *sep = 0;
        create_directory(path);
        *sep = '/';
    }
    if(mkdir(path, 0777) && errno != EEXIST)
        fprintf(stderr, "error while trying to create '%s'\n%m\n", path);
}

void save_labyrinth(const char *fileName, const char *userName, Labyrinth *labyrinth) {
    const char *directory = "labyrinth";
    int result = mkdir(directory, 0777);

    if(result && errno != EEXIST)
        fprintf(stderr, "error while trying to create '%s'\n%m\n", directory); 

    
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "labyrinth/%s", fileName);

    FILE *file = fopen(filepath, "w");

    if (file == NULL) {
        perror("Erreur: impossible de créer le fichier");
        return;
    }
    
    fprintf(file, "Le créateur du labyrinth est: %s\n\n", userName);

    for (int i = 0; i < labyrinth->dim.height; i++) {
        for (int j = 0; j < labyrinth->dim.width; j++) {
            int cell = labyrinth->board[i][j];
            if (cell == WALL) {
                fputs("#", file);
            } else if (cell == START) {
                fputs("o", file);
            } else if (cell == END) {
                fputs("-", file);
            } else {
                fputs(" ", file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    printf("Le fichier \"%s\" est bien sauvegardé\n\n", fileName);

    menu();
}

void display_list_labyrinths() {
    const char *directory = "labyrinth";
    DIR *dir = opendir(directory);
    struct dirent *entry;

    if (dir == NULL) {
        perror("Erreur: impossible d'ouvrir le répertoire");
        return;
    }

    printf("Liste des labyrinthes disponibles :\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Ignore les fichiers cachés
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dir);
}