#include <stdio.h>

void menu() {
    int choice;
    printf(" Voulez-vous :\n");
    printf(" 1. Créer un labyrinthe\n");
    printf(" 2. Charger un labyrinthe\n");
    printf(" 3. Jouer\n");
    printf(" 4. Quitter\n");
    scanf("%d", &choice);

    if(choice == 1) {
        // fonction pour créer un labyrinthe
    } else if(choice == 2) {
       // fonction pour charger un labyrinthe
    } else if(choice == 3) {
         // fonction pour jouer
    } else if(choice == 4) {
       // fonction pour quitter
    } else {
        printf("Choix invalide. Veuillez réessayer.\n");
        menu();
    }

}

int main(int argc, char *argv[]) {
    menu();
    return 0;
}