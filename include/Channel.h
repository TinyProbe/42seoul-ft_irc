#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

class Channel {
 public:
  Channel() {}
  ~Channel() {}

 private:
  std::vector<std::string> op_nick_;
  std::vector<std::string> ch_nick_;
  std::vector<std::string> ban_nick_;
  std::string password_;
  std::string topic_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_