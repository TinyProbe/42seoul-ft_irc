#ifndef IRCSERV_COMMAND_H_
#define IRCSERV_COMMAND_H_

#include <iostream>
#include "Client.h"

namespace irc {

#define RPL_WELCOME "001"
#define RPL_TOPIC "332"
#define ERR_NOSUCHNICK "401"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NICKNAMEINUSE "433"

class Command {
 public:
  Command() {}
  ~Command() {}

  static std::string welcome(Client client_);
  static std::string topic(Client client_, std::string topic);
  static std::string nosuchnick(Client client_, std::string nick_);
  static std::string unknowncommand(Client client_, std::string command);
  static std::string nicknameinuse(Client client_, std::string nick_);

 private:
  std::string ServerAddr(Client client_);
};

} // namespace irc

#endif // IRCSERV_COMMAND_H_