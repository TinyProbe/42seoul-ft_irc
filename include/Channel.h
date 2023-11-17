#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

#include <iostream>
#include <vector>

namespace irc {

class Channel {
 public:
  Channel() {}
  Channel(std::string password)
  :password_(password) {}
  ~Channel() {}

  bool checkBan(std::string nick) const;
  void inputOp(std::string nick);
  void outputOp(std::string nick);
  void inputCh(std::string nick);
  void outputCh(std::string nick);
  void inputBan(std::string nick);
  void outputBan(std::string nick);
  std::vector<std::string> getChNick() const;
  std::string getTopic() const;
  void setTopic(std::string topic);
  std::string getPassword() const;
  void setPassword(std::string password);
  void Channelout(std::string nick);

 private:
  std::vector<std::string> op_nick_;
  std::vector<std::string> ch_nick_;
  std::vector<std::string> ban_nick_;
  std::string password_;
  std::string topic_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_