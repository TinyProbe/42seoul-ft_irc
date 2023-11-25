#ifndef IRCSERV_COMMON_H_
#define IRCSERV_COMMON_H_

// std (C)
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// std (C++)
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <cstddef>

// dev
#include "Channel.h"
#include "Client.h"
#include "EventPool.h"
#include "Program.h"
#include "Request.h"
#include "RequestCallback.h"
#include "RequestPool.h"
#include "Server.h"

namespace irc {

template<typename T> T string_to(std::string const &s) {
  std::stringstream ss;
  T res;
  ss << s;
  ss >> res;
  return res;
}

} // namespace irc

#endif // IRCSERV_COMMON_H_
