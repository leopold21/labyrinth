#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

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

void menu() {
    int choice ;
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
        // fonction pour créer un labyrinthe
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
       // fonction pour charger un labyrinthe
       display_list_labyrinths();
       

    } else if (choice == 3) {
         // fonction pour jouer
      /*  Dimensions dim;
       dim.height = 9;
       dim.width = 9;
       Labyrinth *labyrinth = create_labyrinth(dim);
        
        display_labyrinth(dim, labyrinth->board);*/
    } else if (choice == 4) {
       // fonction pour quitter
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
            } else if (labyrinth[i][j] == START) {
                printf("o "); 
            } else if (labyrinth[i][j] == END) {
                printf("- "); 
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
            if (l[i][j] == -1) {
                printf("# ");
            } else {
                printf("%c ", (char)l[i][j]);
            }
        }
        printf("\n");
    }
}
