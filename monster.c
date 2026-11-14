#include "labyrinth.h"

#include <stdlib.h>

/**
 * @brief Computes neighboring cell coordinates based on a numerical direction index.
 * 
 * @param point Current grid point.
 * @param direction Direction index.
 * @param dim Labyrinth dimensions.
 * 
 * @return Point Target neighbor coordinates.
 */
static Point neighbor(Point point, int direction, Dimensions dim) {
    switch (direction) {
        case 0: return upper_neighbor(point, dim);
        case 1: return lower_neighbor(point, dim);
        case 2: return left_neighbor(point, dim);
        default: return right_neighbor(point, dim);
    }
}

/**
 * @brief Checks if a given point lies within the labyrinth boundary constraints.
 * 
 * @param point Target coordinates to evaluate.
 * @param dim Labyrinth dimensions.
 * 
 * @return true If the point is inside board boundaries, false otherwise.
 */
static bool in_bounds(Point point, Dimensions dim) {
    return point.line >= 0 && point.line < dim.height &&
           point.col >= 0 && point.col < dim.width;
}

/**
 * @brief Calculates the Manhattan distance between two grid points.
 * 
 * @param left First point coordinates.
 * @param right Second point coordinates.
 * 
 * @return int Total Manhattan distance between the two points.
 */
static int distance(Point left, Point right) {
    int rows = abs(left.line - right.line);
    int cols = abs(left.col - right.col);
    return rows + cols;
}

/**
 * @brief Movement strategy routine for Ghost monsters.
 * 
 * @param monster Pointer to the Ghost instance to update.
 * @param labyrinth Pointer to the current Labyrinth state.
 * 
 * @return void
 */
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

/**
 * @brief Movement strategy routine for Ogre monsters.
 * 
 * @param monster Pointer to the Ogre instance to update.
 * @param labyrinth Pointer to the current Labyrinth state.
 * 
 * @return void
 */
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

/**
 * @brief Instantiates and initializes a new Monster structure.
 * 
 * @param point Starting position coordinates for the monster.
 * @param type Monster classification type (GHOST or OGRE).
 * @param penalty_points Score penalty value inflicted on player collision.
 * 
 * @return Monster Initialized Monster object configured with its corresponding movement handler.
 */
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

/**
 * @brief Checks if a specific cell coordinate is currently occupied by an active monster.
 * 
 * @param monsters Array of active Monster structures.
 * @param count Number of elements currently present in the monsters array.
 * @param row Target row index.
 * @param col Target column index.
 * 
 * @return true If a monster already occupies the cell, false otherwise.
 */
static bool monster_occupies(const Monster monsters[], int count, int row, int col) {
    for (int i = 0; i < count; i++) {
        if (monsters[i].point.line == row && monsters[i].point.col == col) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Randomly places monsters onto empty grid cells across the labyrinth board.
 * 
 * @param labyrinth Pointer to the target Labyrinth structure.
 * @param monsters Output array allocated to hold monster instances.
 * @param max_monsters Maximum capacity limit of the monsters array.
 * 
 * @return int Total number of monsters successfully placed.
 */
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
                    monsters[placed] = create_monster(point, type,
                                                      SCORE_MONSTER_PENALTY);
                    placed++;
                    row = labyrinth->dim.height;
                    break;
                }
            }
        }
    }
    return placed;
}
/**
 * @brief Iterates over all active monsters and invokes their respective movement functions.
 * 
 * @param monsters Array containing active Monster instances.
 * @param nb_monsters Count of active monsters in the array.
 * @param labyrinth Pointer to the active Labyrinth instance.
 * 
 * @return void
 */
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