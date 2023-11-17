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

std::string Command::nosuchchannel(Request req, std::string channel_) {
  std::string command;
  
  command = ServerAddr(req) +
            ERR_NOSUCHNICK +
            " " +
            req.getNickname() + 
            " :" + 
            channel_ +
            " No such channel\r\n";
  return command;
}

std::string Command::unknowncommand(Request req, std::string command_) {
  std::string command;
  command_.pop_back();
  command_.pop_back();
  
  command = ServerAddr(req) +
            ERR_UNKNOWNCOMMAND +
            " " +
            req.getNickname() + 
            " :" + 
            command_ +
            "Unknown command\r\n";
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

std::string Command::NotChannelIn(Request req, std::string nick, std::string Channel) {
  std::string command;

  command = ServerAddr(req) +
            ERR_NOTCHANNELIN +
            " " +
            nick +
            " :" +
            nick +
            " " +
            Channel +
            " " +
            "They are not on that channel\r\n";
  return command;
}

std::string Command::kick(Request req) {
  std::string command;
  
  command = ServerAddr(req) +
            " " +
            req.getType();
  return command;
}

std::string Command::command(std::string receive_) {
  int index;
  std::string command;

  for (index = 0; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    command.push_back(receive_[index]);
  }
  return command;
}

std::string Command::firstword(std::string receive_) {
  int index;
  std::string first;

  for (index = 0; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
  }
  for (++index; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    first.push_back(receive_[index]);
  }
  return first;
}

std::string Command::secondword(std::string receive_) {
  int index;
  std::string second;

  for (index = 0; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
  }
  for (++index; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
  }
  for (++index; receive_[index]; ++index) {
    if (!isprint(receive_[index]) || receive_[index] == ' ') {
      break ;
    }
    second.push_back(receive_[index]);
  }
  return second;
}

std::string Command::topic(std::string channel, std::string topic) {
  std::string command;
  
  command = ":Topic PRIVMSG " +
            channel +
            " " +
            topic +
            "\r\n";
  return command;
}

}