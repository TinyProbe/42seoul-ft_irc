#include "Channel.h"

namespace irc {
  bool Channel::checkBan(std::string nick) const {
    for (size_t i = 0; i != ban_nick_.size(); ++i) {
      if (ban_nick_[i] == nick) {
        return true;
      }
    }
    return false;
  }

  void Channel::inputOp(std::string nick) {
    op_nick_.push_back(nick);
  }

  void Channel::outputOp(std::string nick) {
    op_nick_.erase(std::remove(op_nick_.begin(), op_nick_.end(), nick), op_nick_.end());
  }

  void Channel::inputCh(std::string nick) {
    ch_nick_.push_back(nick);
  }

  void Channel::outputCh(std::string nick) {
    ch_nick_.erase(std::remove(ch_nick_.begin(), ch_nick_.end(), nick), ch_nick_.end());
  }

  void Channel::inputBan(std::string nick) {
    ban_nick_.push_back(nick);
  }

  void Channel::outputBan(std::string nick) {
    ban_nick_.erase(std::remove(ban_nick_.begin(), ban_nick_.end(), nick), ban_nick_.end());
  }

  std::string Channel::getTopic() const {
    return topic_;
  }

  void Channel::setTopic(std::string topic) {
    topic_ = topic;
  }

  std::string Channel::getPassword() const {
    return password_;
  }

  void Channel::setPassword(std::string password) {
    password_ = password;
  }

  std::vector<std::string> Channel::getChNick() const {
    return ch_nick_;
  }

  void Channel::Channelout(std::string nick) {
    outputCh(nick);
    outputOp(nick);
  }

}