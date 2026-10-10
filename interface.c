#include <stdio.h>
#include <stdlib.h>
#include "labyrinth.h"
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
            if (game[i][j] == WALL) {
                printf("# "); 
            } else if (game[i][j] == START) {
                printf("o "); 
            } else if (game[i][j] == END) {
                printf("- "); 
            } else if (game[i][j] >= 0 && game[i][j] <= 9) {
                printf("%c ", '0' + game[i][j]); 
            } else if (game[i][j] >= 10 && game[i][j] <= 35) {
                printf("%c ", 'A' + (game[i][j] - 10)); 
            } else {
                printf("%c", game[i][j]); // A voir qu'est-ce qu'on fait
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

