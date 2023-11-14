#include "Channel.h"

namespace irc {

std::string const &Channel::getName() const {
  return name_;
}

void Channel::setName(std::string const &name) {
  name_ = name;
}

void Server::setPassword(std::string const &password) {
  if (password.size() < 6 || password.size() > 16) {
    throw std::runtime_error(std::string("password: 6 <= password <= 16"));
  }
  for (int i = 0; i < password.size(); ++i) {
    if (!isprint(password[i])) {
      throw std::runtime_error(
          std::string("password: format error(unprintable)"));
    }
  }
  password_ = password;
}

bool Channel::verify(std::string const &password) const {
  return password_ == password;
}

Connections &Server::getConnection() const {
  return connection_;
}

Client &Server::getClient(int sock) const {
  Connections::iterator it = connection_.find(sock);
  if (it == connection_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return it->second;
}

Client &Server::getClient(std::string const &nick) const {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it == nick_to_sock_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return connection_[it->second];
}

void Server::disconnect(int sock) {
  Connection::iterator it = connection_.find(sock);
  if (it != connection_.end()) {
    nick_to_sock_.erase(it->second.getNickname());
    connection_.erase(it);
  }
}

void Server::disconnect(std::string const &nick) {
  NickToSock::iterator it = nick_to_sock_.find(nick);
  if (it != nick_to_sock_.end()) {
    connection_.erase(it->second);
    nick_to_sock_.erase(it);
  }
}

} // namespace irc
