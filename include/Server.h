#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

namespace irc {

typedef std::unordered_map<int, Client> Connection;
typedef std::unordered_map<std::string, int> NickToSock;
typedef std::unordered_map<std::string, Channel> ChannelMap;

class Server {
 public:
  Server();
  ~Server() {}

  int getSocket() const;
  std::string const &getHost() const;
  Connection &getConnection() const;
  NickToSock &getNickToSock() const;
  ChannelMap &getChannelMap() const;
  Client &getClient(int sock) const;
  Client &getClient(std::string const &nick) const;
  void setPort(int port);
  void setPassword(std::string const &password);
  bool verify(std::string const &password) const;

  void standby();
  void preProcess();
  int accept();
  void disconnect(int sock);
  void disconnect(std::string const &nick);

  std::vector<Response> const &response(Request const &req);
  bool perform(Response const &res);

 private:
  int         sock_;
  int         port_;
  std::string password_;
  std::string host_;

  Connection connection_;
  NickToSock nick_to_sock_;
  ChannelMap channel_map_;

  RequestCallback  request_callback_;
  ResponseCallback response_callback_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_
