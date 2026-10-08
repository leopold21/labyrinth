#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

void menu() {
    int choice;
    printf(" Voulez-vous :\n");
    printf(" 1. Créer un labyrinthe\n");
    printf(" 2. Charger un labyrinthe\n");
    printf(" 3. Jouer\n");
    printf(" 4. Quitter\n");
    scanf("%d", &choice);

    if (choice == 1) {
        // fonction pour créer un labyrinthe
    } else if (choice == 2) {
       // fonction pour charger un labyrinthe
    } else if (choice == 3) {
         // fonction pour jouer
    } else if (choice == 4) {
       // fonction pour quitter
    } else {
        printf("Choix invalide. Veuillez réessayer.\n");
        menu();
    }
}

Dimensions set_dimensions() {
    Dimensions dim;
    
    do {
        printf("Entrez la hauteur du labyrinthe (impair et >= 3) : ");
        scanf("%d", &dim.height);
        if (dim.height < 3 || dim.height % 2 == 0) {
            printf("Hauteur invalide. Doit être un nombre impair >= 3.\n");
        }
    } while (dim.height < 3 || dim.height % 2 == 0);

    do {
        printf("Entrez la largeur du labyrinthe (impair et >= 3) : ");
        scanf("%d", &dim.width);
        if (dim.width < 3 || dim.width % 2 == 0) {
            printf("Largeur invalide. Doit être un nombre impair >= 3.\n");
        }
    } while (dim.width < 3 || dim.width % 2 == 0);

    return dim;
}

void set_name(char *name) {
    printf("Entrez votre nom : ");
    scanf("%49s", name);
}

void display_labyrinth(Dimensions dim, int game[dim.height][dim.width]) {
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            printf("%c", game[i][j]);
        }
        printf("\n");
    }
}

void initialize_game(Dimensions dim, int game[dim.height][dim.width], PathPoints points) {
    int cpt = 0;

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (i == points.start_line && j == points.start_col) {
                game[i][j] = 'o'; 
            } else if (i == points.end_line && j == points.end_col) {
                game[i][j] = '-'; 
            } else if (i % 2 == 1 && j % 2 == 1 && i < dim.height - 1 && j < dim.width - 1) {
                game[i][j] = '0' + (cpt % 10);
                cpt++;
            } else {
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

void display_found_case(Dimensions dim, int **l) {
    printf("Cases trouvées :\n");
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (l[i][j] == -1) {
                printf("# ");
            } else {
                printf("%c ", (char)l[i][j]);
            }
        }
        printf("\n");
    }
}

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
        if (matrix[i] == NULL) return NULL; // Sécurité si un vecteur échoue
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

int random_selection_case(Dimensions dim, int game[dim.height][dim.width], int *line, int *col) {
    int count = count_found_cases(dim, game);
    if (count == 0) return 0;

    int random_index = rand() % count;
    int current_index = 0;

    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (game[i][j] != '#' && game[i][j] != ' ' && 
                game[i][j] != 'o' && game[i][j] != '-') {
                if (current_index == random_index) {
                    *line = i;
                    *col = j;
                    return 1;
                }
                current_index++;
            }
        }
    }
    return 0;
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

int main(int argc, char *argv[]) {
    Dimensions dim = set_dimensions();
    create_labyrinth(dim);
    return 0;
}