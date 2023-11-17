#include "Request.h"

namespace irc {
  int Request::getRequestCode() const {
    if (Command::command(type_) == "USER") {
      return 1;
    }
    if (Command::command(type_) == "JOIN") {
      return 2;
    }
    if (Command::command(type_) == "NICK") {
      return 3;
    }
    if (Command::command(type_) == "PRIVMSG") {
      return 4;
    }
    if (Command::command(type_) == "KICK") {
      return 5;
    }
    if (Command::command(type_) == "PART") {
      return 6;
    }
    if (Command::command(type_) == "QUIT") {
      return 7;
    }
    if (Command::command(type_) == "TOPIC") {
      return 9;
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