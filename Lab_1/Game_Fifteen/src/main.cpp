#include "FifteenPuzzle.h"
#include <iostream>

int main() {
    FifteenPuzzle game;               // случайная решаемая позиция
    std::cout << game;
    std::cout << "cell[0] = " << game[0]
              << ", solved = " << game.isSolved() << "\n";
}