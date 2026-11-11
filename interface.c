#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "labyrinth.h"

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

static void display_game_labyrinth(Labyrinth labyrinth, const Monster monsters[],
                                   int monster_count) {
    for (int row = 0; row < labyrinth.dim.height; row++) {
        for (int col = 0; col < labyrinth.dim.width; col++) {
            bool monster_found = false;
            for (int i = 0; i < monster_count; i++) {
                if (monsters[i].point.line == row && monsters[i].point.col == col) {
                    printf("%c ", monsters[i].type);
                    monster_found = true;
                    break;
                }
            }
            if (monster_found) {
                continue;
            }

            int cell = labyrinth.board[row][col];
            if (cell == WALL) printf("# ");
            else if (cell == PLAYER) printf("o ");
            else if (cell == END) printf("- ");
            else if (cell == KEY) printf("K ");
            else if (cell == BONUS) printf("X ");
            else if (cell == PENALTY) printf("Y ");
            else printf("  ");
        }
        printf("\n");
    }
}

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
            char nameFile[100];

            if (!read_required_line("Entrez votre nom : ", userName,
                                    sizeof(userName), "Nom trop long.")) {
                free_labyrinth(*labyrinth);
                free(labyrinth);
                return;
            }

            if (!read_required_line("Sous quel nom, voulez-vous sauvegarder le fichier : ",
                                    nameFile, sizeof(nameFile), "Nom de fichier trop long.")) {
                free_labyrinth(*labyrinth);
                free(labyrinth);
                return;
            }

            if (strchr(nameFile, '.') == NULL) {
                strcat(nameFile, ".txt");
            }

            save_labyrinth(nameFile, userName, labyrinth);
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

        printf("----------------------Jeu du labyrinthe----------------------\n");
        printf("Vous êtes représenté par 'o'. Ramassez la clé (K) pour ouvrir la sortie (-).\n");
        printf("Un bonus (X) ajoute 1 point ; un malus (Y) retire 1 point.\n");
        printf("Fantôme (F) : traverse les murs ; ogre (O) : reste dans son territoire.\n");
        printf("Vous pouvez vous déplacer en utilisant les touches 'z' (haut), 's' (bas), 'q' (gauche) et 'd' (droite).\n");
        printf("Bonne chance !\n\n");

        Point current_point = {game.labyrinth.points.start_line, game.labyrinth.points.start_col};
        Point end_point = {game.labyrinth.points.end_line, game.labyrinth.points.end_col};
        Monster monsters[2];
        srand((unsigned int)time(NULL));
        int monster_count = place_monsters(&game.labyrinth, monsters, 2);

        while (current_point.line != end_point.line || current_point.col != end_point.col) {
            display_game_labyrinth(game.labyrinth, monsters, monster_count);
            printf("Score (bonus(X)/malus(Y)): %d | Coups : %d | Clé (K): %s\n",
                   game.score, game.nb_moves, game.is_key_found ? "oui" : "non");
            printf("Entrez votre mouvement (z : haut, s : bas, q : gauche, d : droite) : ");

            char move = 0;
            if (scanf(" %c", &move) != 1) {
                printf("Entrée invalide. Veuillez entrer 'z', 's', 'q' ou 'd'.\n");
                clear_buffer();
                free_labyrinth(game.labyrinth);
                return;
            }
            clear_buffer();

            Point next_point = current_point;
            if (move == 'z') {
                next_point = upper_neighbor(current_point, game.labyrinth.dim);
            } else if (move == 's') {
                next_point = lower_neighbor(current_point, game.labyrinth.dim);
            } else if (move == 'q') {
                next_point = left_neighbor(current_point, game.labyrinth.dim);
            } else if (move == 'd') {
                next_point = right_neighbor(current_point, game.labyrinth.dim);
            } else {
                printf("Entrée invalide. Veuillez entrer 'z', 's', 'q' ou 'd'.\n");
                continue;
            }

            if (next_point.line >= 0 && next_point.line < game.labyrinth.dim.height &&
                next_point.col >= 0 && next_point.col < game.labyrinth.dim.width) {
                int next_cell = game.labyrinth.board[next_point.line][next_point.col];
                if (next_cell == WALL) {
                    printf("Mouvement invalide. Vous ne pouvez pas traverser un mur.\n");
                    continue;
                }
                if (next_cell == END && !game.is_key_found) {
                    printf("La sortie est verrouillée. Trouvez d'abord la clé (K).\n");
                    continue;
                }

                if (next_cell == KEY) {
                    game.is_key_found = true;
                } else if (next_cell == BONUS) {
                    game.score += SCORE_BONUS;
                } else if (next_cell == PENALTY) {
                    game.score += -SCORE_PENALTY;
                }

                game.nb_moves++;
                game.labyrinth.board[current_point.line][current_point.col] = EMPTY;
                current_point = next_point;
                if (current_point.line != end_point.line ||
                    current_point.col != end_point.col) {
                    game.labyrinth.board[current_point.line][current_point.col] = PLAYER;
                }

                bool monster_hit[2] = {false};
                for (int i = 0; i < monster_count; i++) {
                    if (monsters[i].point.line == current_point.line &&
                        monsters[i].point.col == current_point.col) {
                        game.score -= monsters[i].penalty_points;
                        monster_hit[i] = true;
                        printf("Un monstre vous a touché : -%d points.\n",
                               monsters[i].penalty_points);
                    }
                }
                move_all_monsters(monsters, monster_count, &game.labyrinth);
                for (int i = 0; i < monster_count; i++) {
                    if (monsters[i].point.line == current_point.line &&
                        monsters[i].point.col == current_point.col && !monster_hit[i]) {
                        game.score -= monsters[i].penalty_points;
                        printf("Un monstre vous a touché : -%d points.\n",
                               monsters[i].penalty_points);
                    }
                }
            } else {
                printf("Mouvement invalide. Vous ne pouvez pas vous déplacer dans cette direction.\n");
            }
        }

        game.score += SCORE_FACTOR - (game.nb_moves * SCORE_PENALTY_MOVE);
        printf("Bravo, vous avez trouvé la sortie en %d coup(s) ! Score final : %d\n",
               game.nb_moves, game.score);

        char playerName[100];
        if (!read_required_line("Entrez votre nom pour enregistrer votre score : ",
                                playerName, sizeof(playerName), "Nom trop long.")) {
            free_labyrinth(game.labyrinth);
            return;
        }

        save_ranking(fileName, playerName, game.score);
        display_ranking(fileName);
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

void set_name(char *name) {
    read_required_line("Entrez votre nom : ", name, 50, "Nom trop long.");
}


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

void display_labyrinth(Dimensions dim, int **labyrinth) {
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            display_case(labyrinth[i][j]);
        }
        printf("\n");
    }
}

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
