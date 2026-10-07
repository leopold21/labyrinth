#include <stdio.h>
#include <stdlib.h>


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

void display_labyrinth(int height, int width, int game[height][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", game[i][j]);
        }
        printf("\n");
    }
}
void initialize_game(int height, int width, int game[height][width], int start_col, int end_col, int start_line, int end_line) {
    // voir pour afficher quand cpt > 9, pour l'instant on affiche le nombre modulo 10

    int cpt = 0;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {
            if(i == start_line && j == start_col) {
                game[i][j] = 'o'; 
            } else if(i == end_line && j == end_col) {
                game[i][j] = '-'; 
            } else if (i%2 == 1 && j %2 == 1 && i < height - 1 && j < width - 1 ) {
                game[i][j] = '0' + (cpt % 10);
                cpt++;
            }else {
                game[i][j] = '#';
            }
        }
    }
}

void upperNeighbor(const int height, const int width, int game[height][width], int * start_line, const int * start_col) {
    (*start_line)--;
    game[*start_line][*start_col] = (int)' ';

}

void lowerNeighbor(const int height, const int width, int game[height][width], int * start_line, const int * start_col) {
    (*start_line)++;
    game[*start_line][*start_col] = (int)' ';
}

void leftNeighbor(const int height, const int width, int * game[height][width], const int * start_line, int * start_col) {
    (*start_col)--;
    game[*start_line][*start_col] = (int)' ';
}

void rightNeighbor(const int height, const int width, int game[height][width], const int * start_line, int * start_col) {
    (*start_col)++;
    game[*start_line][*start_col] = (int)' ';
}

void find_path(const int height, const int width, int game[height][width], int * start_col, int * end_col, int * start_line, int * end_line) {
    while (start_line != end_line || start_col != end_col) {
        int direction = rand() % 4; 
        switch (direction) {
            case 0: // Up
                if ((*start_line > 0) && game[*start_line - 1][*start_col] != '#') {
                    upperNeighbor(height, width, game, start_line, start_col);
                }
                break;
            case 1: // Down
                if ((*start_line < height - 1) && game[*start_line + 1][*start_col] != '#') {
                    lowerNeighbor(height, width, game, start_line, start_col);
                }
                break;
            case 2: // Left
                if ((*start_col > 0) && game[*start_line][*start_col - 1] != '#') {
                    leftNeighbor(height, width, game, start_line, start_col);
                }
                break;
            case 3: // Right
                if ((*start_col < width - 1) && game[*start_line][*start_col + 1] != '#') {
                    rightNeighbor(height, width, game, start_line, start_col);
                }
                break;
        }
        game[*start_line][*start_col] = (int)'*';
    }
}


void create_labyrinth(int height, int width, int game[height][width]) {
    srand(time(NULL)); 
    int start_col = 1 + 2 * rand() % ((width - 1) / 2);
    int end_col   = 1 + 2 * rand() % ((width - 1) / 2);

    int start_line = 0;
    int end_line = height - 1;
    display_labyrinth(height, width, game);

    printf("Start position: (%d, %d)\n", start_line, start_col);
    initialize_game(height, width, game, start_col, end_col, start_line, end_line);
    
    display_labyrinth(height, width, game);

    printf("end position: (%d, %d)\n", start_line, start_col);
    find_path(height, width, game, start_col, end_col, start_line, end_line);

    display_labyrinth(height, width, game);
}

int main(int argc, char *argv[]) 
{
    const int width = 11; // set_width();
    const int height = 11; // set_height();
    int game[11][11] = {{1}};
    create_labyrinth(width, height, game);
    return 0;
}

