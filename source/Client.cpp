#include "Client.hpp"

std::string &Client::getNickname() const {
  return nickname_;
}

void Client::receive(int socket) {
  char buf[MAX_BUF];
  
  if (recv(socket, buf, MAX_BUF, MSG_DONTWAIT|MSG_NOSIGNAL) == -1) {
    throw std::runtime_error(std::string("recv() error\n") + 
                             std::string(strerror(errno)));
  }
  receive_ += buf;
}

bool Client::canRequest() const {
  if (!receive_.find("\r\n")) { return false; }
  if (nickname_.empty() || usrname_empty()) { 
    certification(receive);
  if (nickname_.empty() || usrname_empty()) { return false; }
  }
  return true;
}

Request Client::createRequest() {
  receive.clear();
}

void Client::setWrite(bool value) {
  write_ = value;
}

bool Client::canWrite() const {
  return write_;
}

void Client::certification(std::string receive) {

}