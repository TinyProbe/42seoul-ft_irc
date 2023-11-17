#ifndef IRCSERV_COMMAND_H_
#define IRCSERV_COMMAND_H_

#include <iostream>
#include "Request.h"

namespace irc {

class Request;

#define RPL_WELCOME "001"
#define RPL_TOPIC "332"
#define ERR_NOSUCHNICK "401"
#define ERR_NOSUCHANNEL "403"
#define ERR_UNKNOWNCOMMAND "421"
#define ERR_NONICKNAME "431"
#define ERR_NICKNAMEINUSE "433"
#define ERR_NOTCHANNELIN "441"
#define ERR_WRONGPASS "473"

class Command {
 public:
  Command() {}
  ~Command() {}

  static std::string ServerAddr(Request req);
  static std::string welcome(Request req);
  static std::string topic(Request req, std::string topic);
  static std::string nosuchnick(Request req, std::string nick_);
  static std::string nosuchchannel(Request req, std::string channel_);
  static std::string unknowncommand(Request req, std::string command);
  static std::string nicknameinuse(Request req, std::string nick_);
  static std::string wrongpass(Request req, std::string password);
  static std::string command(std::string receive_);
  static std::string firstword(std::string receive_);
  static std::string secondword(std::string receive_);
  static std::string Privmsg(Request req);
  static std::string ChangeNick(Request req);
  static std::string NoNickname(Request req);
  static std::string join(Request req);
  static std::string kick(Request req);
  static std::string NotChannelIn(Request req, std::string nick, std::string Channel);
  static std::string topic(std::string channel, std::string topic);

};

} // namespace irc

#endif // IRCSERV_COMMAND_H_