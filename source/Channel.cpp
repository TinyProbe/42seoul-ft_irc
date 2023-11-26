#include "common.h"

namespace irc {

std::string const &Channel::getName() const { return name_; }

void Channel::setName(std::string const &name) { name_ = name; }

void Channel::addInvite(std::string const &nick) { invite_list_[nick] = true; }

void Channel::delInvite(std::string const &nick) { invite_list_.erase(nick); }

bool Channel::isInvited(std::string const &nick) const {
  return invite_list_.find(nick) != invite_list_.end();
}

UMstring_bool &Channel::getJoinedClient() { return joined_client_; }

bool Channel::isJoined(std::string const &nick) const {
  return joined_client_.find(nick) != joined_client_.end();
}

void Channel::join(std::string const &nick) { joined_client_[nick] = true; }

void Channel::part(std::string const &nick) { joined_client_.erase(nick); }

void Channel::setOrigin(std::string const &nick) { origin_ = nick; }

void Channel::addOperator(std::string const &nick) { operator_[nick] = true; }

void Channel::delOperator(std::string const &nick) { operator_.erase(nick); }

int Channel::isOperator(std::string const &nick) const {
  if (origin_ == nick) { return 1; }
  if (operator_.find(nick) != operator_.end()) { return 2; }
  return 0;
}

bool Channel::getInviteOnly() const { return invite_only_; }

bool Channel::getOperTopic() const { return oper_topic_; }

bool Channel::getHasPassword() const { return has_password_; }

bool Channel::getHasLimit() const { return has_limit_; }

void Channel::setInviteOnly(bool invite_only) { invite_only_ = invite_only; }

void Channel::setOperTopic(bool oper_topic) { oper_topic_ = oper_topic; }

void Channel::setHasPassword(bool has_password) {
  has_password_ = has_password;
}

void Channel::setHasLimit(bool has_limit) { has_limit_ = has_limit; }

std::string const &Channel::getTopic() const { return topic_; }

bool Channel::verify(std::string const &password) const {
  return password_ == password;
}

size_t Channel::getLimit() const { return limit_; }

void Channel::setTopic(std::string const &topic) { topic_ = topic; }

void Channel::setPassword(std::string const &password) {
  if (password.size() < 6 || password.size() > 16) {
    throw std::runtime_error("password: 6 <= password <= 16");
  }
  for (int i = 0; i < (int)password.size(); ++i) {
    if (!isprint(password[i])) {
      throw std::runtime_error("password: format error(unprintable)");
    }
  }
  password_ = password;
}

void Channel::setLimit(size_t limit) { limit_ = limit; }

} // namespace irc
