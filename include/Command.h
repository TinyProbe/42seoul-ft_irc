#ifndef IRCSERV_COMMAND_H_
#define IRCSERV_COMMAND_H_

#include <iostream>
#include "Request.h"

namespace irc {

#define RPL_WELCOME "001"
#define RPL_TOPIC "332"
#define ERR_NOSUCHNICK "401"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NONICKNAME "431"
#define ERR_NICKNAMEINUSE "433"
#define ERR_WRONGPASS "473"

class Command {
 public:
  Command() {}
  ~Command() {}

  static std::string ServerAddr(Request req);
  static std::string welcome(Request req);
  static std::string topic(Request req, std::string topic);
  static std::string nosuchnick(Request req, std::string nick_);
  static std::string unknowncommand(Request req, std::string command);
  static std::string nicknameinuse(Request req, std::string nick_);
  static std::string wrongpass(Request req, std::string password);
  static std::string ChName(std::string receive_);
  static std::string ChPassword(std::string receive_);
  static std::string Nick(std::string receive_);
  static std::string PriNick(std::string receive_);
  static std::string Privmsg(Request req);
  static std::string ChangeNick(Request req);
  static std::string NoNickname(Request req);
  static std::string join(Request req);

};

} // namespace irc

#endif // IRCSERV_COMMAND_H_