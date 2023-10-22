#include <iostream>
#include <stdexcept>

#include "Program.h"

int main(int argc, char **argv) {
  try {
    irc::Program().run(argc, argv);
    return EXIT_SUCCESS;
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
