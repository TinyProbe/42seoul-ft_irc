#include "Request.h"

namespace irc {
  int Request::getRequestCode() const {
    if (type_.find("USER") != std::string::npos) {
      return 1;
    }
    if (type_.find("JOIN") != std::string::npos) {
      return 2;
    }
    if (type_.find("NICK") != std::string::npos) {
      return 3;
    }
    if (type_.find("PRIVMSG") != std::string::npos) {
      return 4;
    }
    return 0;
  }

  int Request::getSocket() const {
    return socket_;
  }
  std::string Request::getNickname() const {
    return nickname_;
  }
  std::string Request::getUsrname() const {
    return usrname_;
  }
  std::string Request::getType() const {
    return type_;
  }
}