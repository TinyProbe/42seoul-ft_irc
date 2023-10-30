#ifndef IRCSERV_SERVER_H_
#define IRCSERV_SERVER_H_

namespace irc {

typedef std::unordered_map<int, Client> Connection;
typedef std::unordered_map<std::string, int> NickToSock;

class Server {
 public:
  Server(EventPool &events)
      : sock_(-1), port_(-1),
        request_callback_(*this),
        response_callback_(*this),
        events_(events) {}
  ~Server() {}

  int getSocket() const;
  Connection &getConnection() const;
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

  Response const &response(Request const &req);
  bool perform(Response const &res);

 private:
  EventPool &events_;

  int         sock_;
  int         port_;
  std::string password_;

  Connection connection_;
  NickToSock nick_to_sock_;

  RequestCallback  request_callback_;
  ResponseCallback response_callback_;
};

} // namespace irc

#endif // IRCSERV_SERVER_H_
