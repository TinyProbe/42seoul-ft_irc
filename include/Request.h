#ifndef IRCSERV_REQUEST_H_
#define IRCSERV_REQUEST_H_

#include <vector>
#include <string>

namespace irc {

typedef std::vector<std::string> Vstring;

class Request {
 public:
  Request() : request_code_(),
              requester_sock_(),
              target_(),
              is_derived_() {}
  Request(Request const &rhs) { *this = rhs; }
  Request &operator=(Request const &rhs);
  ~Request() {}

  enum {
    kUnknown = 0,
    kPass,
    kNick,
    kUser,
    kPrivMsg,
    kJoin,
    kNames,
    kPart,
    kKick,
    kInvite,
    kAccept,
    kDeny,
    kTopic,
    kMode,
    kCount,
  };

  int getRequestCode() const;
  int getRequesterSocket() const;
  std::string const &getTarget() const;
  void setRequestCode(int request_code);
  void setRequesterSocket(int requester_sock);
  void setTarget(std::string const &target);

  std::string const &getCommand() const;
  std::string const &getAddi() const;
  Vstring const &getParam() const;
  void setCommand(std::string const &command);
  void setAddi(std::string const &addi);
  void setParam(Vstring const &param);
  void addParam(std::string const &param);

  bool isDerived() const;
  void setDerived(bool is_derived);

 private:
  int         request_code_;
  int         requester_sock_;
  std::string target_;

  std::string command_;
  std::string addi_;
  Vstring     param_;

  bool is_derived_;
};

} // namespace irc

#endif // IRCSERV_REQUEST_H_
