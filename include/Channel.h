#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

typedef std::unordered_map<int, Client> Connection;
typedef std::unordered_map<std::string, int> NickToSock;

class Channel {
 public:
  Channel() {}
  ~Channel() {}

  std::string const &getName() const;
  void setName(std::string const &name);
  void setPassword(std::string const &password);
  bool verify(std::string const &password) const;

  Connection &getConnection() const;
  Client &getClient(int sock) const;
  Client &getClient(std::string const &nick) const;
  void disconnect(int sock);
  void disconnect(std::string const &nick);

 private:
  std::string name_;
  std::string password_;

  Connection connection_;
  NickToSock nick_to_sock_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_
