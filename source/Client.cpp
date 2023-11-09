#include "Client.h"

namespace irc {

std::string Client::getNickname() const {
  return nickname_;
}

void Client::receive(int socket) {
  char buf[MAX_BUF];
  
  socket_ = socket;
  if (recv(socket, buf, MAX_BUF, MSG_DONTWAIT) == -1) {
    throw std::runtime_error(std::string("recv() error\n") + 
                             std::string(strerror(errno)));
  }
  if (receive_.find("\r\n")) {
    receive_.clear();
  }
  receive_ += buf;
  std::cout << "receive : "<< buf;
  memset(buf, 0, MAX_BUF);
}

bool Client::canRequest() const {
  if (!receive_.find("\r\n")) { return false; }
  if (nickname_.empty() || usrname_.empty()) { return false; }
  return true;
}

Request Client::createRequest() {
  std::cout << "createmessage  " << receive_ << std::endl;
  return Request(receive_, nickname_, usrname_, socket_);
}

void Client::setWrite(bool value) {
  write_ = value;
}

bool Client::canWrite() const {
  return write_;
}

void Client::certification() {
  if (!nickname_.empty() && !usrname_.empty() && !password_.empty()) {
    return ;
  }
  if (receive_.find("PASS") != std::string::npos) {
    for (int i = 5; receive_[i]; ++i) {
      if (!isprint(receive_[i]) || receive_[i] == ' ') {
        break ;
      }
      password_.push_back(receive_[i]);
    }
    std::cout << password_ << std::endl;
  }
  if (receive_.find("NICK") != std::string::npos) {
    for (int i = 5; receive_[i]; ++i) {
      if (!isprint(receive_[i]) || receive_[i] == ' ') {
         break ;
      }
      nickname_.push_back(receive_[i]);
    }
    std::cout << nickname_ << std::endl;
  }
  if (receive_.find("USER") != std::string::npos) {
    for (int i = 5; receive_[i]; ++i) {
      if (!isprint(receive_[i]) || receive_[i] == ' ') {
        break;
      }
      usrname_.push_back(receive_[i]);
    }
    std::cout << usrname_ << std::endl;
  }
}

std::string Client::getPassword() const {
  return password_;
}

Client &Client::operator=(const Client &a) {
  if (this != &a) {
    nickname_ = a.nickname_;
    usrname_ = a.usrname_;
    write_ = a.write_;
    receive_ = a.receive_;
    password_ = a.password_;
    socket_ = a.socket_;
  }
  return *this;
}

void Client::setNickname(std::string nick) {
  nickname_ = nick;
}

}