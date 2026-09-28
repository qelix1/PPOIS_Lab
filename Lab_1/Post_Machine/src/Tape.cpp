#include "Tape.h"
#include <iostream>
#include <stdexcept>

int Tape::get(int index) const {
    auto it = cells_.find(index);
    if (it != cells_.end()) {
        return it->second;
    }
    return EMPTY;
}

void Tape::set(int index, int value) {
    if (value != EMPTY && value != MARK) {
        throw std::invalid_argument("Tape value must be 0 (EMPTY) or 1 (MARK)");
    }

    if (value == EMPTY) {
        cells_.erase(index);
    } else {
        cells_[index] = MARK;
    }
}

void Tape::clear() {
    cells_.clear();
}

bool Tape::empty() const {
    return cells_.empty();
}

std::size_t Tape::marks() const {
    return cells_.size();
}

int Tape::maxIndex() const {
    if (cells_.empty()) {
        return -1;
    }

    int maxIdx = -1;
    for (const auto& pair : cells_) {
        if (pair.first > maxIdx) {
            maxIdx = pair.first;
        }
    }
    return maxIdx;
}

bool Tape::operator==(const Tape& rhs) const {
    return cells_ == rhs.cells_;
}

bool Tape::operator!=(const Tape& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const Tape& tape) {
    int maxIdx = tape.maxIndex();
    
    // Если лента пуста, ничего не выводим
    if (maxIdx == -1) {
        return os;
    }

    for (int i = 0; i <= maxIdx; ++i) {
        os << tape.get(i);
        if (i < maxIdx) {
            os << " ";
        }
    }
    return os;
}

std::istream& operator>>(std::istream& is, Tape& tape) {
    Tape temp;
    int val;
    int index = 0;

    while (is >> val) {
        if (val != Tape::EMPTY && val != Tape::MARK) {
            // Если встретили число, отличное от 0 и 1 — это ошибка формата
            is.setstate(std::ios::failbit);
            return is; // Объект tape не изменяется
        }
        temp.set(index++, val);
    }

    // Если цикл завершился из-за конца файла (EOF), это нормально — считывание успешно
    if (is.eof()) {
        tape = temp;
        // Сбрасываем failbit, который мог возникнуть при попытке чтения за пределами файла
        is.clear(is.rdstate() & ~std::ios::failbit);
    }
    
    // Если же произошла ошибка формата (например, буква вместо числа), 
    // failbit уже установлен, и мы просто возвращаем поток, не меняя tape.
    return is;
}