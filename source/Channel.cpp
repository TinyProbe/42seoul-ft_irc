#include "Channel.h"

namespace irc {

std::string const &Channel::getName() const {
  return name_;
}

void Channel::setName(std::string const &name) {
  name_ = name;
}

void Channel::ban(std::string const &nick) {
  disconnect(nick);
  ban_list_[nick] = true;
}

bool Channel::isBanned(std::string const &nick) const {
  return ban_list_[nick];
}

Connection2 &Channel::getConnection2() const {
  return connection2_;
}

bool Channel::isJoined(std::string const &nick) const {
  Connection2::iterator it = connection2_.find(nick);
  if (it == connection2_.end()) {
    throw std::runtime_error(std::string("getClient: invalid key"));
  }
  return it->second;
}

void Channel::disconnect(std::string const &nick) {
  connection2_.erase(nick);
}

void Channel::setOrigin(std::string const &nick) {
  origin_ = nick;
}

void Channel::addOperator(std::string const &nick) {
  operator_[nick] = true;
}

void Channel::delOperator(std::string const &nick) {
  operator_.erase(nick);
}

int Channel::isOperator(std::string const &nick) {
  if (origin_ == nick) { return 1; }
  Operator::iterator it = operator_.find(nick);
  if (it != operator_.end()) { return 2; }
  return 0;
}

bool Channel::getInviteOnly() const { return invite_only_; }

bool Channel::getTopicOnly() const { return topic_only_; }

bool Channel::getHasPassword() const { return has_password_; }

bool Channel::getUserLimit() const { return user_limit_; }

void Channel::setInviteOnly(bool invite_only) { invite_only_ = invite_only; }

void Channel::setTopicOnly(bool topic_only) { topic_only_ = topic_only; }

void Channel::setHasPassword(bool has_password) { has_password_ = has_password; }

void Channel::setUserLimit(bool user_limit) { user_limit_ = user_limit; }

bool Channel::verify(std::string const &password) const {
  return password_ == password;
}

void Channel::setPassword(std::string const &password) {
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

size_t Channel::getLimit() const {
  return limit_;
}

void Channel::setLimit(size_t limit) {
  limit_ = limit;
}

} // namespace irc
