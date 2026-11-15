#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/labyrinth.h"
#include "minunit.h"

MU_TEST(test_labyrinth_generation) {
    Dimensions dim = {7, 7};
    Labyrinth lab = {0};
    lab.dim = dim;
    lab.points.start_line = 0;
    lab.points.start_col = 1;
    lab.points.end_line = 6;
    lab.points.end_col = 5;
    lab.board = allocate_labyrinth(dim);
    mu_assert(lab.board != NULL, "Erreur : allocation plateau");

    initialize_game(lab);
    mu_assert(lab.board[0][1] == PLAYER, "Erreur : position départ joueur");
    mu_assert(lab.board[6][5] == END, "Erreur : position sortie");
    mu_assert(lab.board[0][0] == WALL, "Erreur : mur extérieur");

    generate_path(lab);
    mu_assert(is_generate_path(lab) == true, "Erreur : chemin mal généré");

    for (int r = 0; r < dim.height; r++) {
        for (int c = 0; c < dim.width; c++) {
            if (lab.board[r][c] >= EMPTY) {
                lab.board[r][c] = EMPTY;
            }
        }
    }

    insert_key(lab);
    insert_bonus(lab, 2);
    insert_penalty(lab, 2);

    int keys = 0, bonuses = 0, penalties = 0;
    for (int r = 0; r < dim.height; r++) {
        for (int c = 0; c < dim.width; c++) {
            if (lab.board[r][c] == KEY) keys++;
            else if (lab.board[r][c] == BONUS) bonuses++;
            else if (lab.board[r][c] == PENALTY) penalties++;
        }
    }
    mu_assert_int_eq(1, keys);
    mu_assert_int_eq(2, bonuses);
    mu_assert_int_eq(2, penalties);

    free_labyrinth(lab);
}

MU_TEST(test_ranking) {
    const char *file_labyrinth = "test_ranking.txt";
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s.score", RANKING_DIRECTORY, file_labyrinth);

    /* Nettoyage avant le test pour éviter l'accumulation d'anciens scores */
    remove(filepath);

    save_ranking(file_labyrinth, "Alice", 150);
    save_ranking(file_labyrinth, "Bob", 300);
    save_ranking(file_labyrinth, "Charlie", 200);

    FILE *f = fopen(filepath, "r");
    mu_assert(f != NULL, "Erreur : impossible d'ouvrir le fichier de score");

    Player players[10];
    int n = 0;
    long long saved_at = 0;
    while (n < 10 && fscanf(f, " %49[^:]: %d: %lld", players[n].name, &players[n].score, &saved_at) == 3) {
        n++;
    }
    fclose(f);
    remove(filepath);

    mu_assert(n == 3, "Erreur : nombre de scores lus incorrect");
    mu_assert(strcmp(players[0].name, "Bob") == 0 && players[0].score == 300, "Erreur : 1er du classement");
    mu_assert(strcmp(players[1].name, "Charlie") == 0 && players[1].score == 200, "Erreur : 2e du classement");
    mu_assert(strcmp(players[2].name, "Alice") == 0 && players[2].score == 150, "Erreur : 3e du classement");
}

MU_TEST(test_save_and_load_labyrinth) {
    Dimensions dim = {7, 7};
    Labyrinth lab = {0};
    lab.dim = dim;
    lab.points.start_line = 0;
    lab.points.start_col = 1;
    lab.points.end_line = 6;
    lab.points.end_col = 5;
    lab.board = allocate_labyrinth(dim);
    mu_assert(lab.board != NULL, "Erreur : allocation plateau");

    initialize_game(lab);
    save_labyrinth("test_save_lab.txt", "Alice", &lab);

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/test_save_lab.txt", LABYRINTH_DIRECTORY);

    FILE *f = fopen(filepath, "r");
    mu_assert(f != NULL, "Erreur : le fichier n'a pas été créé");
    if (f) fclose(f);

    Labyrinth loaded = {0};
    int res = load_labyrinth("test_save_lab.txt", &loaded);
    mu_assert(res == 0 || loaded.board != NULL, "Erreur : échec du chargement du labyrinthe");

    if (loaded.board != NULL) {
        free_labyrinth(loaded);
    }
    remove(filepath);
    free_labyrinth(lab);
}

MU_TEST(test_score_bonus_malus) {
    Game game = {0};
    game.labyrinth.dim = (Dimensions){5, 5};
    game.labyrinth.points.start_line = 0;
    game.labyrinth.points.start_col = 1;
    game.labyrinth.points.end_line = 4;
    game.labyrinth.points.end_col = 3;
    game.labyrinth.board = allocate_labyrinth(game.labyrinth.dim);
    initialize_game(game.labyrinth);

    /* Préparation des cases bonus et malus devant le joueur (0,1) */
    game.labyrinth.board[1][1] = BONUS;
    game.labyrinth.board[2][1] = PENALTY;

    /* 
     * Remplacez les deux lignes suivantes par l'appel à votre fonction de déplacement
     * (ex: move_player(&game, 's'); move_player(&game, 's');)
     */
    mu_assert_int_eq(BONUS, game.labyrinth.board[1][1]);
    mu_assert_int_eq(PENALTY, game.labyrinth.board[2][1]);

    free_labyrinth(game.labyrinth);
}

MU_TEST_SUITE(labyrinth_suite) {
    MU_RUN_TEST(test_labyrinth_generation);
    MU_RUN_TEST(test_ranking);
    MU_RUN_TEST(test_save_and_load_labyrinth);
    MU_RUN_TEST(test_score_bonus_malus);
}

int main(void) {
    MU_RUN_SUITE(labyrinth_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}