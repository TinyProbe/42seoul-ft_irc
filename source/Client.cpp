#include "Client.h"

namespace irc {

std::string Client::getNickname() const {
  return nickname_;
}

void Client::receive(int socket) {
  char buf[MAX_BUF];
  
  if (recv(socket, buf, MAX_BUF, MSG_DONTWAIT) == -1) {
    throw std::runtime_error(std::string("recv() error\n") + 
                             std::string(strerror(errno)));
  }
  if (receive_.find("\r\n")) {
    receive_.clear();
  }
  receive_ += buf;
  std::cout << buf << std::endl;
}

bool Client::canRequest() const {
  if (!receive_.find("\r\n")) { return false; }
  if (nickname_.empty() || usrname_.empty()) { return false; }
  return true;
}

Request Client::createRequest() {
  std::cout << "createmessage  " << receive_ << std::endl;
  return Request(receive_);
}

void Client::setWrite(bool value) {
  write_ = value;
}

bool Client::canWrite() const {
  return write_;
}

void Client::certification() {
  char *receive = const_cast<char*>(receive_.c_str());
  char *word = strtok(receive, " ");

  if (!strcmp(word, "PASS")){
    word = strtok(NULL, " ");
    password_ = word;
  }
   if (!strcmp(word, "NICK")){
    word = strtok(NULL, " ");
    nickname_ = word;
  }
   if (!strcmp(word, "USER")){
    word = strtok(NULL, " ");
    usrname_ = word;
  }
}

}