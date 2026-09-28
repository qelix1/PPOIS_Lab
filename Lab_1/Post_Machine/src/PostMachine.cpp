#include "PostMachine.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

// ================= PostCommand =================

bool PostCommand::operator==(const PostCommand& rhs) const {
    return type == rhs.type && arg1 == rhs.arg1 && arg2 == rhs.arg2;
}

bool PostCommand::operator!=(const PostCommand& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const PostCommand& cmd) {
    switch (cmd.type) {
        case PostCommand::Type::MoveRight: os << "> " << cmd.arg1; break;
        case PostCommand::Type::MoveLeft:  os << "< " << cmd.arg1; break;
        case PostCommand::Type::PutMark:   os << "1 " << cmd.arg1; break;
        case PostCommand::Type::EraseMark: os << "0 " << cmd.arg1; break;
        case PostCommand::Type::Branch:    os << "? " << cmd.arg1 << "; " << cmd.arg2; break;
        case PostCommand::Type::Stop:      os << "!"; break;
    }
    return os;
}

// ================= PostMachine =================

// Вспомогательная функция для удаления пробелов по краям строки
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

PostCommand PostMachine::parseCommand(const std::string& line) {
    std::string s = trim(line);
    if (s.empty()) {
        throw std::runtime_error("Empty command line");
    }

    PostCommand cmd;
    char firstChar = s[0];

    if (firstChar == '!') {
        cmd.type = PostCommand::Type::Stop;
        return cmd;
    }

    std::string rest = trim(s.substr(1));

    if (firstChar == '>' || firstChar == '<' || firstChar == '1' || firstChar == '0') {
        if (firstChar == '>') cmd.type = PostCommand::Type::MoveRight;
        else if (firstChar == '<') cmd.type = PostCommand::Type::MoveLeft;
        else if (firstChar == '1') cmd.type = PostCommand::Type::PutMark;
        else if (firstChar == '0') cmd.type = PostCommand::Type::EraseMark;

        std::stringstream ss(rest);
        if (!(ss >> cmd.arg1)) {
            throw std::runtime_error("Invalid command argument: " + line);
        }
    } 
    else if (firstChar == '?') {
        cmd.type = PostCommand::Type::Branch;
        size_t semicolon = rest.find(';');
        if (semicolon == std::string::npos) {
            throw std::runtime_error("Missing ';' in branch command: " + line);
        }
        std::string arg1Str = trim(rest.substr(0, semicolon));
        std::string arg2Str = trim(rest.substr(semicolon + 1));

        std::stringstream ss1(arg1Str);
        std::stringstream ss2(arg2Str);
        if (!(ss1 >> cmd.arg1) || !(ss2 >> cmd.arg2)) {
            throw std::runtime_error("Invalid branch arguments: " + line);
        }
    } 
    else {
        throw std::runtime_error("Unknown command: " + line);
    }

    return cmd;
}

void PostMachine::loadProgram(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open program file: " + filename);
    }

    std::vector<PostCommand> newProgram;
    std::string line;
    int expectedNum = 1;

    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        // Ожидаем формат: "N. команда"
        size_t dotPos = trimmed.find('.');
        if (dotPos == std::string::npos) {
            throw std::runtime_error("Missing dot in line: " + line);
        }

        std::string numStr = trim(trimmed.substr(0, dotPos));
        int num = std::stoi(numStr);
        if (num != expectedNum) {
            throw std::runtime_error("Invalid command numbering. Expected " + std::to_string(expectedNum) + ", got " + std::to_string(num));
        }

        std::string cmdStr = trim(trimmed.substr(dotPos + 1));
        newProgram.push_back(parseCommand(cmdStr));
        expectedNum++;
    }

    if (newProgram.empty()) {
        throw std::runtime_error("Program is empty");
    }

    program_ = std::move(newProgram);
}

void PostMachine::loadTape(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open tape file: " + filename);
    }

    Tape newTape;
    int newHead;
    if (!(file >> newHead)) {
        throw std::runtime_error("Invalid tape file: missing head position");
    }

    int val;
    int index = 0;
    while (file >> val) {
        if (val != Tape::EMPTY && val != Tape::MARK) {
            throw std::runtime_error("Invalid tape value: expected 0 or 1");
        }
        newTape.set(index++, val);
    }

    tape_ = newTape;
    head_ = newHead;
}

void PostMachine::run(int maxSteps) {
    if (program_.empty()) {
        throw std::runtime_error("Program is not loaded");
    }

    int pc = 1; // Program Counter (1-based)
    int steps = 0;

    while (pc >= 1 && pc <= static_cast<int>(program_.size())) {
        if (steps++ >= maxSteps) {
            throw std::runtime_error("Max steps exceeded");
        }

        const PostCommand& cmd = program_[pc - 1];

        switch (cmd.type) {
            case PostCommand::Type::Stop:
                return;

            case PostCommand::Type::MoveRight:
                head_++;
                pc = cmd.arg1;
                break;

            case PostCommand::Type::MoveLeft:
                head_--;
                pc = cmd.arg1;
                break;

            case PostCommand::Type::PutMark:
                if (tape_.get(head_) == Tape::MARK) {
                    throw std::runtime_error("Attempt to put mark on a non-empty cell at index " + std::to_string(head_));
                }
                tape_.set(head_, Tape::MARK);
                pc = cmd.arg1;
                break;

            case PostCommand::Type::EraseMark:
                if (tape_.get(head_) == Tape::EMPTY) {
                    throw std::runtime_error("Attempt to erase an empty cell at index " + std::to_string(head_));
                }
                tape_.set(head_, Tape::EMPTY);
                pc = cmd.arg1;
                break;

            case PostCommand::Type::Branch:
                if (tape_.get(head_) == Tape::EMPTY) {
                    pc = cmd.arg1;
                } else {
                    pc = cmd.arg2;
                }
                break;
        }
    }

    throw std::runtime_error("Jump to non-existent command number");
}

int PostMachine::head() const { return head_; }
const Tape& PostMachine::tape() const { return tape_; }
std::size_t PostMachine::programSize() const { return program_.size(); }

bool PostMachine::operator==(const PostMachine& rhs) const {
    return tape_ == rhs.tape_ && head_ == rhs.head_ && program_ == rhs.program_;
}

bool PostMachine::operator!=(const PostMachine& rhs) const {
    return !(*this == rhs);
}

std::ostream& operator<<(std::ostream& os, const PostMachine& machine) {
    os << machine.head() << "\n" << machine.tape();
    return os;
}

std::istream& operator>>(std::istream& is, PostMachine& machine) {
    PostMachine temp;
    if (is >> temp.head_) {
        if (is >> temp.tape_) {
            machine = temp;
        }
    }
    return is;
}