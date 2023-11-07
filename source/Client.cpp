#include "Client.h"

namespace irc {

static int const kMaxBuffer = (1 << 20);

bool Client::getAuth() const {
  return auth_;
}

int Client::getSocket() const {
  return sock_;
}

struct sockaddr_in const &Client::getAddress() const {
  return addr_;
}

void Client::setAuth(bool auth) {
  auth_ = auth;
}

void Client::setSocket(int sock) {
  sock_ = sock;
}

void Client::setAddress(struct sockaddr_in const &addr) {
  addr_ = addr;
}

std::string const &Client::getNick() const {
  return nick_;
}

std::string const &Client::getUser() const {
  return user_;
}

std::string const &Client::getReal() const {
  return real_;
}

std::string const &Client::getHost() const {
  return host_;
}

void Client::setNick(std::string const &nick) {
  nick_ = nick;
}

void Client::setUser(std::string const &user) {
  user_ = user;
}

void Client::setReal(std::string const &real) {
  real_ = real;
}

void Client::setHost(std::string const &host) {
  host_ = host;
}

std::string &Client::getBuffer() const {
  return buffer_;
}

bool Client::canWrite() const {
  return can_write_;
}

void Client::setWrite(bool can_write) {
  can_write_ = can_write;
}

bool Client::receive() {
  static char buf[kMaxBuffer];
  static int len;
  buffer_.clear();
  while (true) {
    len = recv(sock_, buf, kMaxBuffer - 1, MSG_DONTWAIT | MSG_NOSIGNAL);
    if (len == -1 && errno != EAGAIN) {
      std::cerr << std::string("recv: ") +
                   std::string(strerror(errno)) << std::endl;
      return false;
    }
    buf[std::max(len, 0)] = '\0';
    buffer_ += buf;
    if (len < kMaxBuffer) { return true; }
  }
}

bool Client::makeRequest() {
  // make in request_, return true if possible to make a Request, otherwise
  // false.
}

Request const &Client::getRequest() const {
  return request_;
}

} // namespace irc
