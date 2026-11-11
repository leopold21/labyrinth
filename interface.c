#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "labyrinth.h"

/**
 * @brief Reads a line from standard input.
 * 
 * @param buffer The buffer to store the read line.
 * @param size The size of the buffer.
 * 
 * @return 0 if the end of file is reached, -1 if the line is too long, 1 otherwise.
 */
static int read_line(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;
    }

    char *newline = strchr(buffer, '\n');
    if (newline != NULL) {
        *newline = '\0';
    } else {
        int next = getchar();
        if (next != '\n' && next != EOF) {
            clear_buffer();
            return -1;
        }
    }

    return 1;
}

/**
 *  @brief Reads a required line from standard input.
 * 
 * @param prompt The prompt to display.
 * @param buffer The buffer to store the read line.
 * @param size The size of the buffer.
 * @param tooLongMessage The message to display if the line is too long.
 * 
 * @return 0 if the end of file is reached, -1 if the line is too long, 1 otherwise.
 */
static int read_required_line(const char *prompt, char *buffer, size_t size,
                              const char *tooLongMessage) {
    for (;;) {
        printf("%s", prompt);
        int status = read_line(buffer, size);
        if (status == 0) {
            return 0;
        }
        if (status == -1) {
            printf("%s\n", tooLongMessage);
            continue;
        }

        char *start = buffer;
        while (isspace((unsigned char)*start)) {
            start++;
        }

        char *end = start + strlen(start);
        while (end > start && isspace((unsigned char)end[-1])) {
            end--;
        }
        *end = '\0';

        if (start == end) {
            printf("La saisie ne peut pas être vide. Veuillez réessayer.\n");
            continue;
        }

        if (start != buffer) {
            memmove(buffer, start, strlen(start) + 1);
        }
        return 1;
    }
}

/** @brief Reads an integer from standard input.
 * 
 * @param prompt The prompt to display.
 * @param value The pointer to store the read integer.
 * 
 * @return 0 if the end of file is reached, -1 if the input is invalid, 1 otherwise.
 */
static int read_integer(const char *prompt, int *value) {
    char input[100];

    for (;;) {
        printf("%s", prompt);
        int status = read_line(input, sizeof(input));
        if (status == 0) {
            return 0;
        }
        if (status == -1) {
            printf("Saisie trop longue. Veuillez réessayer.\n");
            continue;
        }

        errno = 0;
        char *end;
        long parsed = strtol(input, &end, 10);
        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (end == input || *end != '\0' || errno == ERANGE ||
            parsed < INT_MIN || parsed > INT_MAX) {
            printf("Veuillez entrer un nombre entier valide.\n");
            continue;
        }

        *value = (int)parsed;
        return 1;
    }
}
/** @brief Loads a labyrinth from a file.
 * 
 * @param labyrinth The labyrinth structure to populate.
 * @param fileName The name of the file to load.
 * @param fileNameSize The size of the file name buffer.
 * 
 * @return 0 if the labyrinth is loaded successfully, -1 otherwise.
 */
static int load_labyrinth_interactively(Labyrinth *labyrinth, char *fileName,
                                       size_t fileNameSize) {
    if (display_list_labyrinths() == -1) {
        return 0;
    }

    if (!read_required_line("Entrez le nom du fichier à charger : ",
                            fileName, fileNameSize,
                            "Nom de fichier trop long.")) {
        return -1;
    }

    return load_labyrinth(fileName, labyrinth);
}

/**
 * @brief Saves a labyrinth to a file, checking for overwrite if the file already exists.
 * 
 * @param userName The name of the user saving the labyrinth.
 * @param labyrinth The labyrinth to save.
 * 
 * @return 0 if the labyrinth is saved successfully, -1 otherwise.
 */
int saveLabyrinth_with_overwrite_check(const char *userName, Labyrinth *labyrinth) {
    char enteredName[200];
    char fileName[205];
    char filepath[256];

    for (;;) {
        if (!read_required_line("Sous quel nom voulez-vous sauvegarder le labyrinthe : ",
                                enteredName, sizeof(enteredName),
                                "Nom de fichier trop long.")) {
            return 0;
        }

        int length;
        if (end_by_txt(enteredName)) {
            length = snprintf(fileName, sizeof(fileName), "%s", enteredName);
        } else {
            length = snprintf(fileName, sizeof(fileName), "%s.txt", enteredName);
        }
        if (length < 0 || (size_t)length >= sizeof(fileName)) {
            printf("Nom de fichier trop long. Veuillez réessayer.\n");
            continue;
        }

        length = snprintf(filepath, sizeof(filepath), "%s/%s",
                          LABYRINTH_DIRECTORY, fileName);
        if (length < 0 || (size_t)length >= sizeof(filepath)) {
            printf("Chemin de fichier trop long. Veuillez choisir un autre nom.\n");
            continue;
        }

        struct stat file_info;
        if (stat(filepath, &file_info) == 0) {
            char answer[16];
            for (;;) {
                if (!read_required_line("Ce fichier existe déjà. Voulez-vous le remplacer ? (o/n) : ",
                                        answer, sizeof(answer),
                                        "Réponse trop longue.")) {
                    return 0;
                }

                if (answer[1] == '\0' &&
                    (answer[0] == 'o' || answer[0] == 'O')) {
                    save_labyrinth(fileName, userName, labyrinth);
                    return 1;
                }
                if (answer[1] == '\0' &&
                    (answer[0] == 'n' || answer[0] == 'N')) {
                    printf("Choisissez un autre nom.\n");
                    break;
                }
                printf("Répondez par o (oui) ou n (non).\n");
            }
            continue;
        }
        if (errno != ENOENT) {
            perror("Impossible de vérifier si le fichier existe");
            return 0;
        }

        save_labyrinth(fileName, userName, labyrinth);
        return 1;
    }
}

/**
 * @brief Reads the player's desired move direction and calculates the next position.
 * 
 * @param current The current position of the player.
 * @param dim The dimensions of the labyrinth.
 * @param next The pointer to store the calculated next position.
 * 
 * @return 0 if the end of file is reached, -1 if the input is invalid, 1 if the move is valid.
 */
 int read_player_destination(Point current, Dimensions dim, Point *next) {
    printf("Entrez votre mouvement (z : haut, s : bas, q : gauche, d : droite) : ");
    char move = 0;
    if (scanf(" %c", &move) != 1) {
        printf("Entrée invalide. Veuillez entrer 'z', 's', 'q' ou 'd'.\n");
        clear_buffer();
        return 0;
    }
    clear_buffer();

    switch (move) {
        case 'z': *next = upper_neighbor(current, dim); return 1;
        case 's': *next = lower_neighbor(current, dim); return 1;
        case 'q': *next = left_neighbor(current, dim); return 1;
        case 'd': *next = right_neighbor(current, dim); return 1;
        default:
            printf("Entrée invalide. Veuillez entrer 'z', 's', 'q' ou 'd'.\n");
            return -1;
    }
}

/**
 * @brief Attempts to move the player to the next position, handling interactions with the labyrinth and monsters.
 * 
 * @param game The current game state.
 * @param current The current position of the player.
 * @param next The desired next position of the player.
 * @param monsters The array of monsters in the labyrinth.
 * @param monster_count The number of monsters in the labyrinth.  
 *   
 * @return true if the move is successful, false otherwise.
 */
static bool try_player_move(Game *game, Point *current, Point next,
                           Monster monsters[], int monster_count) {
    Labyrinth *labyrinth = &game->labyrinth;
    if (next.line < 0 || next.line >= labyrinth->dim.height ||
        next.col < 0 || next.col >= labyrinth->dim.width) {
        printf("Mouvement invalide. Vous ne pouvez pas vous déplacer dans cette direction.\n");
        return false;
    }

    int next_cell = labyrinth->board[next.line][next.col];
    if (next_cell == WALL) {
        printf("Mouvement invalide. Vous ne pouvez pas traverser un mur.\n");
        return false;
    }
    if (next_cell == END && !game->is_key_found) {
        printf("La sortie est verrouillée. Trouvez d'abord la clé (K).\n");
        return false;
    }

    if (next_cell == KEY) {
        game->is_key_found = true;
    } else if (next_cell == BONUS) {
        game->score += SCORE_BONUS;
    } else if (next_cell == PENALTY) {
        game->score -= SCORE_PENALTY;
    }

    game->nb_moves++;
    labyrinth->board[current->line][current->col] = EMPTY;
    *current = next;
    if (current->line != labyrinth->points.end_line ||
        current->col != labyrinth->points.end_col) {
        labyrinth->board[current->line][current->col] = PLAYER;
    }

    move_all_monsters(monsters, monster_count, labyrinth);
    for (int i = 0; i < monster_count; i++) {
        if (monsters[i].point.line == current->line &&
            monsters[i].point.col == current->col) {
            game->score -= monsters[i].penalty_points;
            printf("Un monstre vous a touché : -%d points.\n",
                   monsters[i].penalty_points);
        }
    }
    return true;
}

/**
 * @brief Executes the main loop of the labyrinth game.
 * 
 * @param game Pointer to the structure holding the current game state.
 * @param fileName Name of the labyrinth file currently being played.
 * 
 * @return int 1 if the game finished normally, 0 if interrupted or upon memory allocation failure.
 */
int play_labyrinth(Game *game, const char *fileName) {
    printf("----------------------Jeu du labyrinthe----------------------\n");
    printf("Vous êtes représenté par 'o'. Ramassez la clé (K) pour ouvrir la sortie (-).\n");
    printf("Un bonus (X) ajoute 1 point ; un malus (Y) retire 1 point.\n");
    printf("Fantôme (F) : traverse les murs ; ogre (O) : reste dans son territoire.\n");
    printf("Vous pouvez vous déplacer en utilisant les touches 'z' (haut), 's' (bas), 'q' (gauche) et 'd' (droite).\n");
    printf("Bonne chance !\n\n");

    Point current = {game->labyrinth.points.start_line,
                     game->labyrinth.points.start_col};
    Point end = {game->labyrinth.points.end_line, game->labyrinth.points.end_col};
    int size = game->labyrinth.dim.height * game->labyrinth.dim.width;
    int max_monsters = size / CELL_PER_MONSTER;
    if (max_monsters < 1) {
        max_monsters = 1;
    }

    Monster *monsters = malloc((size_t)max_monsters * sizeof(*monsters));
    if (monsters == NULL) {
        perror("Erreur d'allocation pour les monstres");
        return 0;
    }

    srand((unsigned int)time(NULL));
    int monster_count = place_monsters(&game->labyrinth, monsters, max_monsters);
    while (current.line != end.line || current.col != end.col) {
        display_labyrinth_with_monsters(game->labyrinth, monsters, monster_count);
        printf("Score (bonus(X)/malus(Y)): %d | Coups : %d | Clé (K): %s\n",
               game->score, game->nb_moves, game->is_key_found ? "oui" : "non");

        Point next;
        int input_status = read_player_destination(current, game->labyrinth.dim, &next);
        if (input_status == 0) {
            free(monsters);
            return 0;
        }
        if (input_status < 0) {
            continue;
        }
        try_player_move(game, &current, next, monsters, monster_count);
    }

    game->score += SCORE_FACTOR - (game->nb_moves * SCORE_PENALTY_MOVE);
    printf("Bravo, vous avez trouvé la sortie en %d coup(s) ! Score final : %d\n",
           game->nb_moves, game->score);
    free(monsters);

    char playerName[100];
    if (!read_required_line("Entrez votre nom pour enregistrer votre score : ",
                            playerName, sizeof(playerName), "Nom trop long.")) {
        return 0;
    }

    save_ranking(fileName, playerName, game->score);
    display_ranking(fileName);
    return 1;
}

/**
 * @brief Displays the main menu and handles user selection.
 * 
 * @return void
 */

void menu() {
    int choice = 0;

    printf("---------- Bienvenue dans le jeu du labyrinthe ! ----------\n");
    printf(" Voulez-vous :\n");
    printf(" 1. Créer un labyrinthe\n");
    printf(" 2. Charger un labyrinthe\n");
    printf(" 3. Jouer\n");
    printf(" 4. Quitter\n");

    if (!read_integer("Faites votre choix: ", &choice)) {
        return;
    }

    if (choice == 1) {
        Dimensions dim = set_dimensions();
        if (dim.height == 0) {
            return;
        }

        Labyrinth *labyrinth = create_labyrinth(dim);
        if (labyrinth != NULL) {
            char userName[100];

            if (!read_required_line("Entrez votre nom : ", userName,
                                    sizeof(userName), "Nom trop long.")) {
                free_labyrinth(*labyrinth);
                free(labyrinth);
                return;
            }

            saveLabyrinth_with_overwrite_check(userName, labyrinth);
            free_labyrinth(*labyrinth);
            free(labyrinth);
        }
        menu();

    } else if (choice == 2) {
        Labyrinth labyrinth = {0};
        char fileName[200] = {0};

        int result = load_labyrinth_interactively(&labyrinth, fileName, sizeof(fileName));
        if (result == -1) {
            return;
        }
        if (result == 1) {
            display_labyrinth(labyrinth.dim, labyrinth.board);
            free_labyrinth(labyrinth);
        }
        menu();

    } else if (choice == 3) {
        Game game = {0};
        char fileName[200];

        int result = load_labyrinth_interactively(&game.labyrinth, fileName, sizeof(fileName));
        if (result == -1) {
            return;
        }
        if (result == 0) {
            menu();
            return;
        }

        if (!play_labyrinth(&game, fileName)) {
            free_labyrinth(game.labyrinth);
            return;
        }
        free_labyrinth(game.labyrinth);
        menu();
    } else if (choice == 4) {
        printf("Merci d'avoir joué ! À bientôt !\n");
        return;
    } else {
        printf("Choix invalide. Le choix doit être un nombre entier compris entre 1 et 4.\n\n");
        menu();
    }
}

/**
 * @brief Prompts for and validates the dimensions (height and width) for a new labyrinth. 
 * 
 * @return Dimensions containing the chosen height and width
 */
Dimensions set_dimensions() {
    Dimensions dim = {0, 0};

    while(1) {
        if (!read_integer("Entrez la hauteur du labyrinthe (impair et >= 5) : ", &dim.height)) { // permet au programme de ne pas planter si l'utilisateur entre un caractère non numérique par rapport à un scanf
            return dim;
        }
        if (dim.height < 5 || dim.height % 2 == 0) {
            printf("Hauteur invalide. Doit être un nombre impair >= 5.\n");
            continue;
        }
        break;
    }

    while(1){
        if (!read_integer("Entrez la largeur du labyrinthe (impair et >= 5) : ", &dim.width)) { 
            dim.height = 0;
            return dim;
        }
        if (dim.width < 5 || dim.width % 2 == 0) {
            printf("Largeur invalide. Doit être un nombre impair >= 5.\n");
            continue;
        }
        break;
    }

    return dim;
}

/**
 * @brief Prompts for the player's name via standard input.
 * 
 * @param name Output buffer for storing the player name.
 * 
 * @return void
 */
void set_name(char *name) {
    read_required_line("Entrez votre nom : ", name, 50, "Nom trop long.");
}

/**
 * @brief Prints a single formatted character corresponding to a labyrinth cell type.
 * 
 * @param cell Integer value representing the cell type.
 * 
 * @return void
 */

static void display_case(int cell) {
    if (cell == WALL) {
        printf("# ");
    } else if (cell == PLAYER) {
        printf("o ");
    } else if (cell == END) {
        printf("- ");
    } else if (cell == KEY) {
        printf("K ");
    } else if (cell == BONUS) {
        printf("X ");
    } else if (cell == PENALTY) {
        printf("Y ");
    } else {
        printf("  ");
    }
}

/**
 * @brief Renders the entire labyrinth grid in the console.
 * 
 * @param dim Dimensions of the labyrinth (height and width).
 * @param labyrinth 2D array representing the game board.
 * 
 * @return void
 */
void display_labyrinth(Dimensions dim, int **labyrinth) {
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            display_case(labyrinth[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Renders the labyrinth grid while overlaying monster positions.
 *
 * @param labyrinth Labyrinth structure containing board data.
 * @param monsters Array of active Monster structures.
 * @param count Total number of monsters in the array.
 * @return void
 */
void display_labyrinth_with_monsters(Labyrinth labyrinth,
                                     const Monster *monsters, int count) {
    for (int row = 0; row < labyrinth.dim.height; row++) {
        for (int col = 0; col < labyrinth.dim.width; col++) {
            bool monster_found = false;
            for (int i = 0; i < count; i++) {
                if (monsters[i].point.line == row && monsters[i].point.col == col) {
                    printf("%c ", monsters[i].type == GHOST ? 'F' : 'O');
                    monster_found = true;
                    break;
                }
            }
            if (!monster_found) {
                display_case(labyrinth.board[row][col]);
            }
        }
        printf("\n");
    }
}

/**
 * @brief Renders the grid, casting unknown cell values directly to characters.
 * 
 * @param dim Dimensions of the labyrinth.
 * @param l 2D array representing the game board.
 * 
 * @return void
 */
void display_found_case(Dimensions dim, int **l) {
    printf("Cases trouvées :\n");
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            switch (l[i][j]) {
                case WALL:
                    printf("# ");
                    break;
                case PLAYER:
                    printf("o ");
                    break;
                case END:
                    printf("- ");
                    break;
                case KEY:
                    printf("K ");
                    break;
                case BONUS:
                    printf("X ");
                    break;
                case PENALTY:
                    printf("Y ");
                    break;
                case EMPTY:
                    printf("  ");
                    break;
                default:
                    printf("%c ", (char)l[i][j]);
                    break;
            }
        }
        printf("\n");
    }
}
