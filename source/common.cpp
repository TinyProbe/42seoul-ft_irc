#include "common.h"

#include <signal.h>

#include <iostream>

namespace irc {

void sigpipe(int sig) {
  if (sig == SIGPIPE) {
    std::cerr << "SIGPIPE occured" << std::endl;
  }
}

} // namespace irc
