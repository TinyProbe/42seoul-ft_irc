#include "Program.h"

int main(int argc, char **argv) {
  irc::Program::init(argc, argv);
  irc::Program::run();
}
