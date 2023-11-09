#include "Command.h"

namespace irc {

std::string Command::ServerAddr(Request req) { 
   std::string addr = ":" +
                      req.getNickname() + 
                      "!" +
                      req.getUsrname() +
                     "@127.0.0.1 "; 
  return addr;
}

std::string Command::welcome(Request req) {
  std::string command;
  
  command = ServerAddr(req) +
            RPL_WELCOME +
            " " +
            req.getNickname() + 
            " :Welcome to the IRC server\r\n";
  return command;
}

std::string Command::topic(Request req, std::string topic) {
  std::string command;
  
  command = ServerAddr(req) +
            RPL_TOPIC +
            req.getNickname() + 
            " :Topic: " + 
            topic +
            "\r\n";
  return command;
}

std::string Command::nosuchnick(Request req, std::string nick_) {
  std::string command;
  
  command = ServerAddr(req) +
            ERR_NOSUCHNICK +
            " " +
            req.getNickname() + 
            " :" + 
            nick_ +
            " No such nick\r\n";
  return command;
}

std::string Command::unknowncommand(Request req, std::string command_) {
  std::string command;
  
  command = ServerAddr(req) +
            ERR_UNKNOWNCOMMAND +
            req.getNickname() + 
            " :" + 
            command_ +
            " Unknown command\r\n";
  return command;
}

std::string Command::nicknameinuse(Request req, std::string nick_) {
  std::string command;
  
  command = ServerAddr(req) +
            ERR_NICKNAMEINUSE +
            " " +
            req.getNickname() + 
            " :" + 
            nick_ +
            " Nickname is already in use.\r\n";
  return command;
}

std::string Command::wrongpass(Request req, std::string password) {
  std::string command;
  
  command = ServerAddr(req) +
            ERR_WRONGPASS +
            " " +
            req.getNickname() + 
            " :" + 
            password +
            " password is wrong.\r\n";
  return command;
}

std::string Command::ChName(std::string receive_) {
  int index;
  std::string name;

  for (index = 5; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    name.push_back(receive_[index]);
  }
  return name;
}

std::string Command::ChPassword(std::string receive_) {
  int index;
  std::string password;

  for (index = 5; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
  }
  for (++index; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
      password.push_back(receive_[index]);
  }
  return password;
}

std::string Command::Nick(std::string receive_) {
  int index;
  std::string nick;

  for (index = 5; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    nick.push_back(receive_[index]);
  }
  return nick;
}

std::string Command::PriNick(std::string receive_) {
  int index;
  std::string nick;

  for (index = 8; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    nick.push_back(receive_[index]);
  }
  return nick;
}

std::string Command::join(Request req) {
  std::string command;
  
  command = ServerAddr(req) +
            " " +
            req.getType();
  return command;
}

std::string Command::ChangeNick(Request req) {
  std::string command;
  
  command = ServerAddr(req) +
           " " +
            req.getType();
  return command;
}

std::string Command::Privmsg(Request req) {
  std::string command;
  
  command = ServerAddr(req) +
           " " +
            req.getType();
  return command;
}

std::string Command::NoNickname(Request req) {
  std::string command;

  command = ServerAddr(req) +
            ERR_NONICKNAME +
            " " +
            req.getNickname() + 
            " :No nickname given\r\n";
  return command;
}

}