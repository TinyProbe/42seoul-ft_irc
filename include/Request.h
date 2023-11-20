#ifndef IRCSERV_REQUEST_H_
#define IRCSERV_REQUEST_H_

namespace irc {

class Request {
 public:
  Request() {}
  ~Request() {}

  Request(std::string buffer, int socket);
  int getRequestCode() const;
  int getRequesterSocket() const;
  int getTargetSocket() const;
  std::string getCommand() const;
  std::vector<std::string> getParam() const;
  int seperate(std::string command);

  static const int kUnknown = 0;
  static const int kPass    = 1;
  static const int kNick    = 2;
  static const int kUser    = 3;
  static const int kPrivMsg = 4;
  static const int kJoin    = 5;
  static const int kNames   = 6;
  static const int kPart    = 7;
  static const int kKick    = 8;
  static const int kInvite  = 9;
  static const int kTopic   = 10;
  static const int kMode    = 11;
  static const int kCount   = 12;

 private:
  std::string command_;ring> param_;
  int code_;
  std::vector<std::st
  int socket_;
  bool derive;
};

} // namespace irc

#endif // IRCSERV_REQUEST_H_
