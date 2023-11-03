#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

class Channel {
 public:
  Channel() {}
  ~Channel() {}

  bool checkBan(std::string nick);
  void inputOp(std::string nick);
  void outputOp(std::string nick);
  void inputCh(std::string nick);
  void outputCh(std::string nick);
  void inputBan(std::string nick);
  void outputBan(std::string nick);
  std::string getTopic() const;
  void setTopic(std::string topic);
  std::string getPassword() const;

 private:
  std::vector<std::string> op_nick_;
  std::vector<std::string> ch_nick_;
  std::vector<std::string> ban_nick_;
  std::string password_;
  std::string topic_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_