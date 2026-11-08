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

static int load_labyrinth_interactively(Labyrinth *labyrinth) {
    char fileName[200];

    if (display_list_labyrinths() == -1) {
        return 0;
    }

    clear_buffer();
    if (!read_required_line("Entrez le nom du fichier à charger : ",
                            fileName, sizeof(fileName) - 4,
                            "Nom de fichier trop long.")) {
        return -1;
    }

    return load_labyrinth(fileName, labyrinth);
}

void menu() {
    int choice ;
    printf("---------- Bienvenue dans le jeu du labyrinthe ! ----------\n");
    printf(" Voulez-vous :\n");
    printf(" 1. Créer un labyrinthe\n");
    printf(" 2. Charger un labyrinthe\n");
    printf(" 3. Jouer\n");
    printf(" 4. Quitter\n");
    printf("Faites votre choix: ");

    if(scanf("%d", &choice) != 1){
        printf("Choix invalide, le choix doit être un nombre entier compris entre 1 et 4");
        clear_buffer();
        menu();
        return;
    }
    if (choice == 1) {
        // Créer un labyrinthe
        clear_buffer();
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
                return;
            }

            if (!read_required_line("Sous quel nom, voulez-vous sauvegarder le fichier: ",
                                    nameFile, sizeof(nameFile) - 4, "Nom de fichier trop long.")) {
                free_labyrinth(*labyrinth);
                return;
            }
            strcat(nameFile, ".txt");
            save_labyrinth(nameFile, userName, labyrinth);
            free_labyrinth(*labyrinth);
        }

    } else if (choice == 2) {
        // Charger un labyrinthe
        Labyrinth labyrinth = {0};
        int result = load_labyrinth_interactively(&labyrinth);
        if (result == -1) {
            return;
        }
        if (result == 1) {
            display_labyrinth(labyrinth.dim, labyrinth.board);
            free_labyrinth(labyrinth);
        }
        menu();

    } else if (choice == 3) {
        // Jouer
        Game game = {0};
        int result = load_labyrinth_interactively(&game.labyrinth);
        if (result == -1) {
            return;
        }
        if (result == 0) {
            menu();
            return;
        }

        printf("----------------------Jeu du labyrinthe----------------------\n");
        printf("Vous êtes représenté par 'o'. Ramassez la clé (K) pour ouvrir la sortie (-).\n");
        printf("Un bonus (B) ajoute 1 point ; un malus (M) retire 1 point.\n");
        printf("Vous pouvez vous déplacer en utilisant les touches 'z' (haut), 's' (bas), 'q' (gauche) et 'd' (droite).\n");
        printf("Bonne chance !\n\n");

        Point current_point = {game.labyrinth.points.start_line, game.labyrinth.points.start_col};
        Point end_point = {game.labyrinth.points.end_line, game.labyrinth.points.end_col};

        while (current_point.line != end_point.line || current_point.col != end_point.col) {
            display_labyrinth(game.labyrinth.dim, game.labyrinth.board);
            printf("Score (bonus/malus): %d | Coups : %d | Clé : %s\n",
                   game.score, game.nb_moves, game.is_key_found ? "oui" : "non");
            printf("Entrez votre mouvement (z : haut, s : bas, q : gauche, d : droite) : ");
            char move;
            if (scanf(" %c", &move) != 1) {
                printf("Entrée invalide. Veuillez entrer 'z', 's', 'q' ou 'd'.\n");
                free_labyrinth(game.labyrinth);
                return;
            }

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
            } else {
                printf("Mouvement invalide. Vous ne pouvez pas vous déplacer dans cette direction.\n");
            }
        }
        game.score += SCORE_FACTOR - (game.nb_moves * SCORE_PENALTY_MOVE); 
        printf("Bravo, vous avez trouvé la sortie en %d coup(s) ! Score final : %d\n",
               game.nb_moves, game.score);
        free_labyrinth(game.labyrinth);
        menu();
    } else if (choice == 4) {
        // Quitter
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


void display_labyrinth(Dimensions dim, int ** labyrinth) {
    for (int i = 0; i < dim.height; i++) {
        for (int j = 0; j < dim.width; j++) {
            if (labyrinth[i][j] == WALL) {
                printf("# "); 
            } else if (labyrinth[i][j] == PLAYER) {
                printf("o "); 
            } else if (labyrinth[i][j] == END) {
                printf("- "); 
            } else if (labyrinth[i][j] == KEY) {
                printf("K ");
            } else if (labyrinth[i][j] == BONUS) {
                printf("B ");
            } else if (labyrinth[i][j] == PENALTY) {
                printf("M ");
            }
           else{
            printf("  ");
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
                    printf("B ");
                    break;
                case PENALTY:
                    printf("M ");
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
