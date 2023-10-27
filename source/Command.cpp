#include "Command.h"

std::string Command::ServerAddr(Client client_) { 
   std::string addr = ":" +
                      client_.nickname_ + 
                      "!" +
                      client_.usrname_ +
                     "@127.0.0.1 "; 
  return addr;
}

std::string Command::welcome(Client client_) {
  std::string command;
  
  command = ServerAddr(client_) +
            RPL_WELCOME +
            client_.nickname_ + 
            " :Welcome to the IRC server\r\n";
  return command;
}

std::string Command::topic(Client client_, std::string topic) {
  std::string command;
  
  command = ServerAddr(client_) +
            RPL_TOPIC +
            client_.nickname_ + 
            " :Topic: " + 
            topic +
            "\r\n";
  return command;
}

std::string Command::nosuchnick(Client client_, std::string nick_) {
  std::string command;
  
  command = ServerAddr(client_) +
            ERR_NOSUCHNICK +
            client_.nickname_ + 
            " :" + 
            nick_ +
            " No such nick\r\n";
  return command;
}

std::string Command::unknowncommand(Client client_, std::string command_) {
  std::string command;
  
  command = ServerAddr(client_) +
            ERR_UNKNOWNCOMMAND +
            client_.nickname_ + 
            " :" + 
            command_ +
            " Unknown command\r\n";
  return command;
}

std::string Command::nicknameinuse(Client client_, std::string nick_) {
    std::string command;
  
  command = ServerAddr(client_) +
            ERR_NICKNAMEINUSE +
            client_.nickname_ + 
            " :" + 
            nick_ +
            " Nickname is already in use.\r\n";
  return command;
}

