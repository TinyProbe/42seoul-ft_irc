#include "Program.h"

#include <iostream> // std::cout

namespace irc {

void Program::init(int argc, char **argv) {
  Program::argc_ = argc;
  Program::argv_ = argv;
}

void Program::run() {
  for (int i = 0; i < Program::argc_; ++i) {
    std::cout << Program::argv_[i] << '\n';
  }
}

int Program::argc_;
char **Program::argv_;

} // namespace irc
