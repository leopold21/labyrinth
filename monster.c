#include "labyrinth.h"

#include <stdlib.h>

static Point neighbor(Point point, int direction, Dimensions dim) {
    switch (direction) {
        case 0: return upper_neighbor(point, dim);
        case 1: return lower_neighbor(point, dim);
        case 2: return left_neighbor(point, dim);
        default: return right_neighbor(point, dim);
    }
}

static bool in_bounds(Point point, Dimensions dim) {
    return point.line >= 0 && point.line < dim.height &&
           point.col >= 0 && point.col < dim.width;
}

static int distance(Point left, Point right) {
    int rows = abs(left.line - right.line);
    int cols = abs(left.col - right.col);
    return rows + cols;
}

static void move_ghost(Monster *monster, Labyrinth *labyrinth) {
    int first_direction = rand() % 4;
    for (int offset = 0; offset < 4; offset++) {
        Point destination = neighbor(monster->point, (first_direction + offset) % 4,
                                     labyrinth->dim);
        if (in_bounds(destination, labyrinth->dim) &&
            labyrinth->board[destination.line][destination.col] != END &&
            distance(destination, monster->territory_center) <= monster->penalty_points) {
            monster->point = destination;
            return;
        }
    }
}

static void move_ogre(Monster *monster, Labyrinth *labyrinth) {
    int first_direction = rand() % 4;
    for (int offset = 0; offset < 4; offset++) {
        Point destination = neighbor(monster->point, (first_direction + offset) % 4,
                                     labyrinth->dim);
        if (!in_bounds(destination, labyrinth->dim) ||
            labyrinth->board[destination.line][destination.col] == WALL ||
            labyrinth->board[destination.line][destination.col] == END ||
            distance(destination, monster->territory_center) > monster->penalty_points) {
            continue;
        }

        monster->point = destination;
        int cell = labyrinth->board[destination.line][destination.col];
        if (cell == KEY || cell == BONUS || cell == PENALTY) {
            monster->territory_center = destination;
        }
        return;
    }
}

Monster create_monster(Point point, enum type_monster type, int penalty_points) {
    Monster monster = {0};
    monster.point = point;
    monster.territory_center = point;
    monster.type = type;
    monster.penalty_points = penalty_points;

    if (type == GHOST) {
        monster.move = move_ghost;
    } else if (type == OGRE) {
        monster.move = move_ogre;
    } else {
        monster.move = NULL;
    }
    return monster;
}

static bool monster_occupies(const Monster monsters[], int count, int row, int col) {
    for (int i = 0; i < count; i++) {
        if (monsters[i].point.line == row && monsters[i].point.col == col) {
            return true;
        }
    }
    return false;
}

int place_monsters(Labyrinth *labyrinth, Monster monsters[], int max_monsters) {
    if (labyrinth == NULL || labyrinth->board == NULL ||
        monsters == NULL || max_monsters <= 0) {
        return 0;
    }

    int placed = 0;
    while (placed < max_monsters) {
        int available = 0;
        for (int row = 0; row < labyrinth->dim.height; row++) {
            for (int col = 0; col < labyrinth->dim.width; col++) {
                if (labyrinth->board[row][col] == EMPTY &&
                    !monster_occupies(monsters, placed, row, col)) {
                    available++;
                }
            }
        }
        if (available == 0) {
            break;
        }

        int target = rand() % available;
        for (int row = 0; row < labyrinth->dim.height; row++) {
            for (int col = 0; col < labyrinth->dim.width; col++) {
                if (labyrinth->board[row][col] != EMPTY ||
                    monster_occupies(monsters, placed, row, col)) {
                    continue;
                }
                if (target-- == 0) {
                    Point point = {row, col};
                    enum type_monster type = placed == 0 ? GHOST : OGRE;
                    monsters[placed] = create_monster(point, type, 3);
                    placed++;
                    row = labyrinth->dim.height;
                    break;
                }
            }
        }
    }
    return placed;
}

void move_all_monsters(Monster monsters[], int nb_monsters, Labyrinth *labyrinth) {
    if (monsters == NULL || labyrinth == NULL) {
        return;
    }
    for (int i = 0; i < nb_monsters; i++) {
        if (monsters[i].move != NULL) {
            monsters[i].move(&monsters[i], labyrinth);
        }
    }
}