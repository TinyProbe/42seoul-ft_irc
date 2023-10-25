#ifndef IRCSERV_CLIENT_H_
#define IRCSERV_CLIENT_H_

#define MAX_BACKLOG 1024

namespace irc {

class Client {
 public:
  Client() {
    nikcname_.clear();
    usrname_.clear();
    receive_.clear();
    write(false);
  }
  ~Client() {}

  std::string &getNickname() const;
  void receive(int socket);
  bool canRequest() const ; 
  Request createRequest();
  void setWrite(bool value);

 private:
	std::string nickname_;
	std::string usrname_;
	bool write_;
    std::string receive_;
    void certification(std::string receive);
};

} // namespace irc

#endif // IRCSERV_CLIENT_H_