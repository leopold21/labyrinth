#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "labyrinth.h"

/**
 * @brief Initializes the labyrinth by setting up the board with walls, empty spaces, player, and end point.
 * 
 * @param labyrinth The labyrinth to initialize.
 * 
 * @return void
 */

void initialize_game(Labyrinth labyrinth) {
    int cpt = 0;

    for (int i = 0; i < labyrinth.dim.height; i++) {
        for (int j = 0; j < labyrinth.dim.width; j++) {
            if (i == labyrinth.points.start_line && j == labyrinth.points.start_col) {
                labyrinth.board[i][j] = PLAYER;
            } else if (i == labyrinth.points.end_line && j == labyrinth.points.end_col) {
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

/**
 * @brief Inserts a specified number of cases of a given type into the labyrinth at random empty locations.
 * 
 * @param labyrinth The labyrinth where cases will be inserted.
 * @param type The type of case to insert
 * @param count The number of cases to insert.
 * 
 * @return The number of cases  inserted.
 */

int insert_cases(Labyrinth labyrinth, int type, int count) {
    int available = 0;
    for (int row = 0; row < labyrinth.dim.height; row++) {
        for (int col = 0; col < labyrinth.dim.width; col++) {
            if (labyrinth.board[row][col] == EMPTY) {
                available++;
            }
        }
    }

    if (count > available) {
        count = available;
    }

    for (int i = 0; i < count; i++) {
        int target = rand() % available;
        for (int row = 0; row < labyrinth.dim.height; row++) {
            for (int col = 0; col < labyrinth.dim.width; col++) {
                if (labyrinth.board[row][col] == EMPTY && target-- == 0) {
                    labyrinth.board[row][col] = type;
                    available--;
                    row = labyrinth.dim.height;
                    break;
                }
            }
        }
    }

    return count;
}

/**
 * @brief Inserts a key into the labyrinth at a random empty location.
 * 
 * @param labyrinth The labyrinth where the key will be inserted.
 * 
 * @return void
 */

void insert_key(Labyrinth labyrinth) {
    (void)insert_cases(labyrinth, KEY, 1);
}

/**
 * @brief Inserts bonuses into the labyrinth at random empty locations.
 * 
 * @param labyrinth The labyrinth  where bonuses will be inserted.
 * @param count The number of bonuses to insert.
 * 
 * @return void
 */
void insert_bonus(Labyrinth labyrinth, int count) {
    insert_cases(labyrinth, BONUS, count);
}

/**
 * @brief Inserts penalties into the labyrinth at random empty locations.
 * 
 * @param labyrinth The labyrinth where penalties will be inserted.
 * @param count The number of penalties to insert.
 * 
 * @return void
 */
void insert_penalty(Labyrinth labyrinth, int count) {
    insert_cases(labyrinth, PENALTY, count);
}

/**
 * @brief Allocates memory for a labyrinth.
 * 
 * @param dim The dimensions of the labyrinth.
 * 
 * @return A pointer to the allocated labyrinth, or NULL if allocation fails.
 */
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

/**
 * @brief Frees the memory allocated for a labyrinth.
 * 
 * @param labyrinth The labyrinth to free.
 * 
 * @return void
 */
void free_labyrinth(Labyrinth labyrinth) {
    if (labyrinth.board == NULL) return;
    for (int i = 0; i < labyrinth.dim.height; i++) {
        free(labyrinth.board[i]);
    }
    free(labyrinth.board);
}

/**
 * @brief Checks if a cell is walkable.
 * 
 * @param point The point representing the cell to check.
 * @param labyrinth The labyrinth.
 * 
 * @return true if the cell is walkable, false otherwise.
 */
bool is_walkable_cell(Point point, Labyrinth labyrinth) {
    if (point.line < 0 || point.line >= labyrinth.dim.height ||
        point.col < 0 || point.col >= labyrinth.dim.width) {
        return false;
    }
    int val_point = labyrinth.board[point.line][point.col];
    return val_point != WALL && val_point != PLAYER && val_point != END;
}

/**
 * @brief Checks if the cell above the given point is walkable.
 * 
 * @param point The point representing the cell to check.
 * @param labyrinth The labyrinth.
 * 
 * @return true if the cell above is walkable, false otherwise.
 */
bool is_upper_cell_walkable(Point point, Labyrinth labyrinth) {
    Point upper_point = {point.line - 1, point.col};
    return is_walkable_cell(upper_point, labyrinth);
}

/**
 * @brief Checks if the cell below the given point is walkable.
 * 
 * @param point The point representing the cell to check.
 * @param labyrinth The labyrinth.
 * 
 * @return true if the cell below is walkable, false otherwise.
 */
bool is_lower_cell_walkable(Point point, Labyrinth labyrinth) {
    Point lower_point = {point.line + 1, point.col};
    return is_walkable_cell(lower_point, labyrinth);
}

/**
 * @brief Checks if the cell to the left of the given point is walkable.
 * 
 * @param point The point representing the cell to check.
 * @param labyrinth The labyrinth.
 * 
 * @return true if the cell to the left is walkable, false otherwise.
 */
bool is_left_cell_walkable(Point point, Labyrinth labyrinth) {
    Point left_point = {point.line, point.col - 1};
    return is_walkable_cell(left_point, labyrinth);
}

/**
 * @brief Checks if the cell to the right of the given point is walkable.
 * 
 * @param point The point representing the cell to check.
 * @param labyrinth The labyrinth.
 * 
 * @return true if the cell to the right is walkable, false otherwise.
 */
bool is_right_cell_walkable(Point point, Labyrinth labyrinth) {
    Point right_point = {point.line, point.col + 1};
    return is_walkable_cell(right_point, labyrinth);
}

/**
 * @brief Returns the point representing the cell above the given point.
 * 
 * @param point The point representing the cell to check.
 * @param dim The dimensions of the labyrinth.
 * 
 * @return The point representing the cell above.
 */
Point upper_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point upper_point = {point.line - 1, point.col};
    return upper_point;
}

/**
 * @brief Returns the point representing the cell below the given point.
 * 
 * @param point The point representing the cell to check.
 * @param dim The dimensions of the labyrinth.
 * 
 * @return The point representing the cell below.
 */
Point lower_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point lower_point = {point.line + 1, point.col};
    return lower_point;
}

/**
 * @brief Returns the point representing the cell to the left of the given point.
 * 
 * @param point The point representing the cell to check.
 * @param dim The dimensions of the labyrinth.
 * 
 * @return The point representing the cell to the left.
 */
Point left_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point left_point = {point.line, point.col - 1};
    return left_point;
}

/**
 * @brief Returns the point representing the cell to the right of the given point.
 * 
 * @param point The point representing the cell to check.
 * @param dim The dimensions of the labyrinth.
 * 
 * @return The point representing the cell to the right.
 */
Point right_neighbor(Point point, Dimensions dim) {
    (void)dim;
    Point right_point = {point.line, point.col + 1};
    return right_point;
}

/**
 * @brief Selects a random wall cell for path generation.
 * 
 * @param labyrinth The labyrinth.
 * 
 * @return The point representing the selected wall cell.
 */
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

/**
 * @brief Merges the components of two cells.
 * 
 * @param labyrinth The labyrinth.
 * @param p1 The first point.
 * @param p2 The second point.
 * @param p3 The third point.
 * 
 * @return void
 */
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

/**
 * @brief Checks if a path can be generated in the labyrinth.
 * 
 * @param labyrinth The labyrinth.
 * 
 * @return true if a path can be generated, false otherwise.
 */
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

/**
 * @brief Generates a path in the labyrinth.
 * 
 * @param labyrinth The labyrinth.
 * 
 * @return void
 */
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

/**
 * @brief Checks if a wall cell is broken.
 * 
 * @param labyrinth The labyrinth.
 * @param row The row of the cell to check.
 * @param col The column of the cell to check.
 * 
 * @return true if the wall is broken, false otherwise.
 */
bool wall_is_broken(const Labyrinth *labyrinth, int row, int col) {
    return labyrinth->board[row][col] == WALL &&
           ((row % 2 == 0 && col % 2 == 1) || (row % 2 == 1 && col % 2 == 0));
}

/**
 * @brief Counts the number of breakable walls in the labyrinth
 * .
 * @param labyrinth The labyrinth.
 * 
 * @return The number of breakable walls.
 */
int count_walls_breakable(Labyrinth *labyrinth) {
    int count = 0;
    for (int i = 1; i < labyrinth->dim.height - 1; i++) {
        for (int j = 1; j < labyrinth->dim.width - 1; j++) {
            if (wall_is_broken(labyrinth, i, j)) {
                count++;
            }
        }
    }
    return count;
}

/**
 * @brief Removes walls from the labyrinth.
 * 
 * @param labyrinth The labyrinth.
 * 
 * @return void
 */
void remove_walls(Labyrinth *labyrinth) {
    int total_walls_broken = count_walls_breakable(labyrinth);
    int walls_to_remove = total_walls_broken / WALL_REMOVAL_DIVISOR;

    while (walls_to_remove > 0) {
        int row = 1 + rand() % (labyrinth->dim.height - 2);
        int col = 1 + rand() % (labyrinth->dim.width - 2);

        if (wall_is_broken(labyrinth, row, col)) {
            labyrinth->board[row][col] = EMPTY;
            walls_to_remove--;
        }
    }
}

/**
 * @brief Creates a labyrinth with the specified dimensions and initializes it.
 * 
 * @param dim The dimensions of the labyrinth.
 * 
 * @return A pointer to the created labyrinth, or NULL if creation fails.   
 */ 
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

    int level;
    printf("Choisissez le niveau de difficulté du labyrinthe :\n");
    printf("1. Facile\n");
    printf("2. Difficile\n");

    while (1) {
        printf("Entrez votre choix (1 ou 2) : ");
        if (scanf("%d", &level) != 1) {
            if (feof(stdin)) {
                free_labyrinth(*labyrinth);
                free(labyrinth);
                return NULL;
            }
            clear_buffer();
            printf("Choix invalide. Veuillez entrer 1 pour Facile ou 2 pour Difficile.\n");
            continue;
        }
        clear_buffer();
        if (level == EASY || level == HARD) {
            break;
        } else {
            printf("Choix invalide. Veuillez entrer 1 pour Facile ou 2 pour Difficile.\n");
        }
    }
    labyrinth->points.start_col = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    labyrinth->points.end_col = 1 + 2 * (rand() % ((dim.width - 1) / 2));
    labyrinth->points.start_line = 0;
    labyrinth->points.end_line = dim.height - 1;

    initialize_game(*labyrinth);
    generate_path(*labyrinth);
    if (level == HARD) {
        remove_walls(labyrinth);
    }

    int available_cells = 0;
    for (int row = 0; row < dim.height; row++) {
        for (int col = 0; col < dim.width; col++) {
            int cell = labyrinth->board[row][col];
            if (cell >= EMPTY) {
                labyrinth->board[row][col] = EMPTY;
                available_cells++;
            }
        }
    }

    if (available_cells == 0) {
        fprintf(stderr, "Le labyrinthe ne contient aucune case disponible.\n");
        free_labyrinth(*labyrinth);
        free(labyrinth);
        return NULL;
    }

    insert_key(*labyrinth);
    int item_count = dim.height * dim.width / 20;
    insert_bonus(*labyrinth, item_count);
    insert_penalty(*labyrinth, item_count);

    return labyrinth;
}

/**
 * @brief Creates a directory at the specified path, including any necessary parent directories.
 * 
 * @param path The path of the directory to create.
 * 
 * @return void
 */
void create_directory(char *path) {
    char *sep = strrchr(path, '/');
    if(sep != NULL) {
        *sep = 0;
        create_directory(path);
        *sep = '/';
    }
    if(mkdir(path, 0777) && errno != EEXIST)
        fprintf(stderr, "impossible de créer le dossier '%s'\n%m\n", path);
}

/**
 * @brief Saves the labyrinth to a file.
 * 
 * @param fileName The name of the file to save the labyrinth to.
 * @param userName The name of the user who created the labyrinth.
 * @param labyrinth The labyrinth to save.
 * 
 * @return void
 */
void save_labyrinth(const char *fileName, const char *userName, Labyrinth *labyrinth) {
    int result = mkdir(LABYRINTH_DIRECTORY, 0777);

    if(result && errno != EEXIST)
        fprintf(stderr, "impossible de créer le dossier'%s'\n%m\n", LABYRINTH_DIRECTORY);

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", LABYRINTH_DIRECTORY, fileName);

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
            } else if (cell == PLAYER) {
                fputs("o", file);
            } else if (cell == END) {
                fputs("-", file);
            } else if (cell == KEY) {
                fputs("K", file);
            } else if (cell == BONUS) {
                fputs("X", file);
            } else if (cell == PENALTY) {
                fputs("Y", file);
            } else {
                fputs(" ", file);
            }
        }
        fputc('\n', file);
    }

    fclose(file);
    printf("Le fichier \"%s\" est bien sauvegardé\n\n", fileName);
}

/**
 * @brief Displays the list of available labyrinths.
 * 
 * @return 0 on success, -1 on failure.
 */
int display_list_labyrinths() {
    const char *directory = LABYRINTH_DIRECTORY;
    DIR *dir = opendir(directory);
    struct dirent *entry;

    if (dir == NULL) {
        printf("Aucun labyrinthe créé, veuillez en créer un.\n");
        return -1;
    }

    printf("Liste des labyrinthes disponibles :\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Ignore les fichiers cachés
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dir);
    return 0;
}

/**
 * @brief Checks if a file name ends with ".txt".
 * 
 * @param fileName The name of the file to check.
 * 
 * @return 1 if the file name ends with ".txt", 0 otherwise.
 */
int end_by_txt(const char *fileName) {
    size_t len = strlen(fileName);
    return (len >= 4 && strcmp(fileName + len - 4, ".txt") == 0);
}

/**
 * @brief Parses and loads a labyrinth grid structure from a saved text file.
 * 
 * @param fileName Name of file to open.
 * @param labyrinth Pointer to target Labyrinth structure to store board data.
 * @return int 1 if loading and validation succeed, 0 on format error or missing file.
 */
int load_labyrinth(const char *fileName, Labyrinth *labyrinth) {
    const char *directory = LABYRINTH_DIRECTORY;
    char filePath[256];
    if (fileName == NULL || labyrinth == NULL) {
        fprintf(stderr, "Nom de fichier ou labyrinthe invalide.\n");
        return 0;
    }

    if(end_by_txt(fileName)) {
        snprintf(filePath, sizeof(filePath), "%s/%s", directory, fileName);
    } else {
        snprintf(filePath, sizeof(filePath), "%s/%s.txt", directory, fileName);
    }

    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Le fichier \"%s\" n'existe pas.\n", fileName);
        return 0;
    }

    char *line = NULL;
    size_t capacity = 0;
    ssize_t length;
    int height = 0;
    int width = 0;
    int start_count = 0;
    int end_count = 0;
    int key_count = 0;
    int valid = 1;

    if (getline(&line, &capacity, file) == -1) {
        valid = 0;
    }

    while (valid && (length = getline(&line, &capacity, file)) != -1) {
        while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
            line[--length] = '\0';
        }
        if (length == 0) {
            continue;
        }

        if (length > INT_MAX || (width != 0 && width != length) || height == INT_MAX) {
            valid = 0;
            break;
        }
        if (width == 0) {
            width = (int)length;
        }

        for (ssize_t col = 0; col < length; col++) {
            if (line[col] == 'o') {
                start_count++;
            } else if (line[col] == '-') {
                end_count++;
            } else if (line[col] == 'K') {
                key_count++;
            } else if (line[col] != '#' && line[col] != ' ' &&
                       line[col] != 'X' && line[col] != 'Y' &&
                       line[col] != 'B' && line[col] != 'M') {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            break;
        }
        height++;
    }

    if (ferror(file) || height == 0 || width == 0 ||
        start_count != 1 || end_count != 1 || key_count > 1) {
        valid = 0;
    }

    Labyrinth loaded = {0};
    if (valid) {
        loaded.dim.height = height;
        loaded.dim.width = width;
        loaded.board = allocate_labyrinth(loaded.dim);
        if (loaded.board == NULL) {
            valid = 0;
        }
    }

    if (valid) {
        rewind(file);
        if (getline(&line, &capacity, file) == -1) {
            valid = 0;
        }
        int row = 0;
        while (valid && (length = getline(&line, &capacity, file)) != -1) {
            while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
                line[--length] = '\0';
            }
            if (length == 0) {
                continue;
            }

            for (int col = 0; col < width; col++) {
                switch (line[col]) {
                    case '#':
                        loaded.board[row][col] = WALL;
                        break;
                    case 'o':
                        loaded.board[row][col] = PLAYER;
                        loaded.points.start_line = row;
                        loaded.points.start_col = col;
                        break;
                    case '-':
                        loaded.board[row][col] = END;
                        loaded.points.end_line = row;
                        loaded.points.end_col = col;
                        break;
                    case 'K':
                        loaded.board[row][col] = KEY;
                        break;
                    case 'X':
                    case 'B':
                        loaded.board[row][col] = BONUS;
                        break;
                    case 'Y':
                    case 'M':
                        loaded.board[row][col] = PENALTY;
                        break;
                    default:
                        loaded.board[row][col] = 0;
                        break;
                }
            }
            row++;
        }
        if (valid && row == height) {
            if (key_count == 0) {
                insert_key(loaded);
                int has_key = 0;
                for (int loaded_row = 0; loaded_row < height; loaded_row++) {
                    for (int col = 0; col < width; col++) {
                        if (loaded.board[loaded_row][col] == KEY) {
                            has_key = 1;
                        }
                    }
                }
                if (!has_key) {
                    valid = 0;
                }
            }
        }
        if (valid && row == height) {
            *labyrinth = loaded;
        } else {
            valid = 0;
        }
    }

    if (!valid) {
        if (loaded.board != NULL) {
            free_labyrinth(loaded);
        }
        fprintf(stderr, "Le fichier \"%s\" ne contient pas un labyrinthe valide.\n", fileName);
    }

    free(line);
    fclose(file);
    return valid;
}

/**
 * @brief Compares two players based on their scores, saved times, and names for sorting purposes.
 * 
 * @param left Pointer to the first player.
 * @param right Pointer to the second player.
 * 
 * @return A negative value if the first player should come before the second, a positive value if the first player should come after the second, or zero if they are equal.
 */

int compare_players_by_score(const void *left, const void *right) {
    const Player *player_left = (const Player *)left;
    const Player *player_right = (const Player *)right;

    if (player_left->score < player_right->score) {
        return 1;
    }
    if (player_left->score > player_right->score) {
        return -1;
    }
    if (player_left->saved_at < player_right->saved_at) {
        return -1;
    }
    if (player_left->saved_at > player_right->saved_at) {
        return 1;
    }
    return strcmp(player_left->name, player_right->name);
}

/**
 * @brief Reads the ranking file and populates the players array with player information.
 * 
 * @param filepath The path to the ranking file.
 * @param players The array of Player to populate.
 * @param max_players The maximum number of players to read.
 * 
 * @return The number of players read from the file.    
 */
int read_ranking_file(const char *filepath, Player *players, int max_players) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return 0;
    }

    int count = 0;
    char line[256];
    while (count < max_players && fgets(line, sizeof(line), file) != NULL) {
        char name[50];
        int score = 0;
        long long saved_at = 0;

        if (sscanf(line, "%49[^:]: %d: %lld", name, &score, &saved_at) == 3) {
            snprintf(players[count].name, sizeof(players[count].name), "%s", name);
            players[count].score = score;
            players[count].saved_at = (time_t)saved_at;
            count++;
        } else if (sscanf(line, "%49[^:]: %d", name, &score) == 2) {
            snprintf(players[count].name, sizeof(players[count].name), "%s", name);
            players[count].score = score;
            players[count].saved_at = 0;
            count++;
        }
    }

    fclose(file);
    return count;
}

/**
 * @brief Displays the ranking for a given file.
 * 
 * @param fileName The name of the file for which to display ranking.
 * 
 * @return void
 */
void display_ranking(const char *fileName) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.score", RANKING_DIRECTORY, fileName);

    Player players[200] = {0};
    int count = read_ranking_file(filepath, players, 200);
    if (count == 0) {
        printf("Aucun classement disponible pour le fichier \"%s\".\n", fileName);
        return;
    }

    qsort(players, (size_t)count, sizeof(Player), compare_players_by_score);
    if (count > MAX_RANKING) {
        count = MAX_RANKING;
    }

    printf("Classement pour le fichier \"%s\":\n", fileName);
    for (int i = 0; i < count; i++) {
        printf("%d. %s : %d\n", i + 1, players[i].name, players[i].score);
    }
}

/**
 * @brief Creates the ranking directory if it does not exist.
 * 
 * @return void
 */
void create_ranking_directory() {
    int result = mkdir(LABYRINTH_DIRECTORY, 0777);
    if (result && errno != EEXIST) {
        fprintf(stderr, "Impossible de créer le dossier '%s'\n%m\n", LABYRINTH_DIRECTORY);
    }

    result = mkdir(RANKING_DIRECTORY, 0777);
    if (result && errno != EEXIST) {
        fprintf(stderr, "Impossible de créer le dossier '%s'\n%m\n", RANKING_DIRECTORY);
    }
}

/**
 * @brief Saves the ranking for a given file.
 * 
 * @param fileName The name of the file for which to save ranking.
 * @param userName The name of the user to add to the ranking.
 * 
 * @param score The score of the user.
 */
void save_ranking(const char *fileName, const char *userName, int score) {
    create_ranking_directory();

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.score", RANKING_DIRECTORY, fileName);

    Player players[201] = {0};
    int count = read_ranking_file(filepath, players, 200);

    Player new_player = {0};
    snprintf(new_player.name, sizeof(new_player.name), "%s", userName);
    new_player.score = score;
    new_player.saved_at = time(NULL);
    players[count] = new_player;
    count++;

    qsort(players, (size_t)count, sizeof(Player), compare_players_by_score);
    if (count > MAX_RANKING) {
        count = MAX_RANKING;
    }

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Erreur: impossible de créer le fichier de classement");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s: %d: %lld\n", players[i].name, players[i].score,
                (long long)players[i].saved_at);
    }
    fclose(file);
}