#include "Request.h"

namespace irc {

  Request::request(std::string buffer, int socket) {
    std::stringstream command(buffer);
    std::string word;

    getline(command, word, ' ');
    command_ = word;
    code = seperate(word);
    getline(command, word, ' ')
    std:stringstream target(word);
    while (getline(target, word, ',')) {
      param_.push_back(word);
    }
    getline(command, word, '\0');
    param_.push_back(word);
    socket_ = socket;
    derive = false;
  }

  int Request::getRequestCode() const {
    return code;
  }

  int Request::getRequesterSocket() const {
    return socket;
  }

  int Request::getTargetSocket() const {
    return socket;
  }

  std::string Request::getCommand() const {
    return command;
  }

  std::vector<std::string> Request::getParam() const {
    return param_;
  }

  int Request::seperate(std::string command) {
    if (command == "PASS") {
      return 1;
    } else if (command == "NICK") {
      return 2;
    } else if (command == "USER") {
      return 3;
    } else if (command == "PRIVMSG") {
      return 4;
    } else if (command == "JOIN") {
      return 5;
    } else if (command == "NAMES") {
      return 6;
    } else if (command == "PART") {
      return 7;
    } else if (command == "KICK") {
      return 8;
    } else if (command == "INVITE") {
      return 9;
    } else if (command == "TOPIC") {
      return 10;
    } else if (command == "MODE") {
      return 11;
    } else {
      return 0;
    }
  }

  bool Request::isDerived()const {
    return derive_;
  }

  Request &Request::operator= (const Request &other) {
    if (this != &other) {
      command_ = other.command_;
      code_ = other.code_;
      param_ = other.param_;
      socket_ = other.socket_;
      derive_ = other.derive_
    }
  }
} // namespace irc
