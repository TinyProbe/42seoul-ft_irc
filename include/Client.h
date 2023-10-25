#ifndef IRCSERV_CLIENT_H_
#define IRCSERV_CLIENT_H_

namespace irc {

class Client {
 public:
  Client() {}
  ~Client() {}

  std::string &getNickname(void) const;
  void receive(int socket);
  bool canRequest() const ; 
  Request createRequest();
  void setWrite(bool value);

 private:
	std::string nickname_;
	std::string id_;
	std::string realname_;
	bool write;
};

} // namespace irc

#endif // IRCSERV_CLIENT_H_