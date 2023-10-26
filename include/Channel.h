#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

class Channel {
 public:
  Channel() {}
  ~Channel() {}

  bool check_ban(std::string nick);
  void input_op(std::string nick);
  void output_op(std::string nick);
  void input_ch(std::string nick);
  void output_ch(std::string nick);
  void input_ban(std::string nick);
  void output_ban(std::string nick);
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