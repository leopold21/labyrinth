#include "labyrinth.h"

int main(int argc, char *argv[]) {
    Dimensions dim = set_dimensions();
    create_labyrinth(dim);
    return 0;
}