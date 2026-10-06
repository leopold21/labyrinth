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

int set_height() {
    int height;
    printf("Entrez la hauteur du labyrinthe : ");
    scanf("%d", &height);
    return height;
}

int set_width() {
    int width;
    printf("Entrez la largeur du labyrinthe : ");
    scanf("%d", &width);
    return width;
}


void set_name(char * name) {
    printf("Entrez votre nom : ");
    scanf("%49s", name); // 49s, car la taille du tableau est de 50, et on laisse une place pour le caractère nul
}

void display_labyrinth() {
    int height = set_height(), width = set_width();
    char name[50];
    set_name(name);
    printf("Labyrinthe de %s :\n", name);
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            printf("#");
        }
        printf("\n");
    }
}
void create_labyrinth() {
    int width = set_width();
    int height = set_height();
    int size = width * height;
    int game[height][width];
    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            game[i][j] = '#';
        }
        printf("\n");
    }
    int start_col = rand() % width;
    int end_col = rand() % width;

    game[0][start_col] = 'o';           
    game[height - 1][end_col] = '-';    
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", game[i][j]);
        }
        printf("\n");
    }



}

int main(int argc, char *argv[]) {
    create_labyrinth();
    return 0;
}

