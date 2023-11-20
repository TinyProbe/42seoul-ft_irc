#include "Channel.h"

namespace irc {

std::string const &Channel::getName() const {
  return name_;
}

void Channel::setName(std::string const &name) {
  name_ = name;
}

void Channel::ban(std::string const &nick) {
  part(nick);
  ban_list_[nick] = true;
}

bool Channel::isBanned(std::string const &nick) const {
  return ban_list_[nick];
}

Joined &Channel::getJoined() const {
  return joined_;
}

bool Channel::isJoined(std::string const &nick) const {
  Connection2::iterator it = joined_.find(nick);
  if (it == joined_.end()) {
    return false;
  }
  return true;
}

bool Channel::join(std::string const &nick) {
  if (isBanned(nick)) { return false; }
  return (joined_[nick] = true);
}

void Channel::part(std::string const &nick) {
  joined_.erase(nick);
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

bool Channel::getOperTopic() const { return oper_topic_; }

bool Channel::getHasPassword() const { return has_password_; }

bool Channel::getUserLimit() const { return user_limit_; }

void Channel::setInviteOnly(bool invite_only) { invite_only_ = invite_only; }

void Channel::setOperTopic(bool oper_topic) { oper_topic_ = oper_topic; }

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
