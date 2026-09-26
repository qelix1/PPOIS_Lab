#include "FifteenPuzzle.h"

#include <algorithm>
#include <cstdlib>
#include <istream>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>

namespace {

std::mt19937& rng() {
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

} 


FifteenPuzzle::FifteenPuzzle() {
    shuffle();
}

FifteenPuzzle::FifteenPuzzle(const std::array<int, CELLS>& cells) {
    std::array<bool, CELLS> seen{};
    for (int v : cells) {
        if (v < 0 || v >= CELLS || seen[static_cast<std::size_t>(v)])
            throw std::invalid_argument(
                "FifteenPuzzle: расстановка должна быть перестановкой 0..15");
        seen[static_cast<std::size_t>(v)] = true;
    }
    board_    = cells;
    emptyPos_ = static_cast<int>(
        std::find(board_.begin(), board_.end(), EMPTY) - board_.begin());
}


FifteenPuzzle::FifteenPuzzle(const FifteenPuzzle& other)
    : board_(other.board_), emptyPos_(other.emptyPos_) {}

FifteenPuzzle& FifteenPuzzle::operator=(const FifteenPuzzle& other) {
    if (this != &other) {
        board_    = other.board_;
        emptyPos_ = other.emptyPos_;
    }
    return *this;
}


void FifteenPuzzle::shuffle() {
    for (int i = 0; i < CELLS - 1; ++i) board_[i] = i + 1;
    board_[CELLS - 1] = EMPTY;

    do {
        std::shuffle(board_.begin(), board_.end(), rng());
    } while (!isSolvable(board_));

    emptyPos_ = static_cast<int>(
        std::find(board_.begin(), board_.end(), EMPTY) - board_.begin());
}

bool FifteenPuzzle::isSolvable(const std::array<int, CELLS>& b) {
    int inversions = 0;
    for (int i = 0; i < CELLS; ++i) {
        if (b[i] == EMPTY) continue;
        for (int j = i + 1; j < CELLS; ++j) {
            if (b[j] == EMPTY) continue;
            if (b[i] > b[j]) ++inversions;
        }
    }

    const int emptyIndex = static_cast<int>(
        std::find(b.begin(), b.end(), EMPTY) - b.begin());
    const int rowFromBottom = SIZE - emptyIndex / SIZE;

    return (inversions % 2) != (rowFromBottom % 2);
}

bool FifteenPuzzle::isAdjacent(int a, int b) {
    const int dr = std::abs(a / SIZE - b / SIZE);
    const int dc = std::abs(a % SIZE - b % SIZE);
    return dr + dc == 1;
}

bool FifteenPuzzle::move(int row, int col) {
    if (row < 0 || row >= SIZE || col < 0 || col >= SIZE)
        return false;

    const int index = row * SIZE + col;
    if (!isAdjacent(index, emptyPos_))
        return false;

    std::swap(board_[index], board_[emptyPos_]);
    emptyPos_ = index;
    return true;
}

int FifteenPuzzle::operator[](int index) const {
    if (index < 0 || index >= CELLS)
        throw std::out_of_range("FifteenPuzzle::operator[]: неверный индекс");
    return board_[index];
}

bool FifteenPuzzle::isSolved() const {
    for (int i = 0; i < CELLS - 1; ++i)
        if (board_[i] != i + 1)
            return false;
    return true;
}


bool FifteenPuzzle::operator==(const FifteenPuzzle& rhs) const {
    return board_ == rhs.board_;
}

bool FifteenPuzzle::operator!=(const FifteenPuzzle& rhs) const {
    return !(*this == rhs);
}


std::ostream& operator<<(std::ostream& os, const FifteenPuzzle& p) {
    for (int r = 0; r < FifteenPuzzle::SIZE; ++r) {
        for (int c = 0; c < FifteenPuzzle::SIZE; ++c) {
            if (c > 0) os << ' ';
            os << p[r * FifteenPuzzle::SIZE + c];
        }
        os << '\n';
    }
    return os;
}

std::istream& operator>>(std::istream& is, FifteenPuzzle& p) {
    std::array<int, FifteenPuzzle::CELLS> cells{};
    for (int& v : cells)
        if (!(is >> v))
            return is;

    try {
        p = FifteenPuzzle(cells);
    } catch (const std::exception&) {
        is.setstate(std::ios::failbit);
    }
    return is;
}