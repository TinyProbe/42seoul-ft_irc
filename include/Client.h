#ifndef IRCSERV_CLIENT_H_
#define IRCSERV_CLIENT_H_

namespace irc {

class Client {
 public:
  Client() : auth_(), sock_(-1), len_(), can_write_() {
    bzero(&addr_, sizeof(struct sockaddr_in));
  }
  ~Client() {}

  bool getAuth() const;
  int getSocket() const;
  struct sockaddr_in const &getAddress() const;
  void setAuth(bool auth);
  void setSocket(int sock);
  void setAddress(struct sockaddr_in const &addr);

  std::string const &getNick() const;
  std::string const &getUser() const;
  std::string const &getReal() const;
  std::string const &getHost() const;
  void setNick(std::string const &nick);
  void setUser(std::string const &user);
  void setReal(std::string const &real);
  void setHost(std::string const &host);

  std::string &getBuffer() const;
  bool canWrite() const;
  void setWrite(bool can_write);

  bool receive();
  bool makeRequest();
  Request const &getRequest() const;

 private:
  bool               auth_;
  int                sock_;
  struct sockaddr_in addr_;

  std::string nick_;
  std::string user_;
  std::string real_;
  std::string host_;

  std::string buffer_;
  bool        can_write_;

  Request request_;
};

} // namespace irc

#endif // IRCSERV_CLIENT_H_
