#ifndef IRCSERV_REQUEST_H_
#define IRCSERV_REQUEST_H_

namespace irc {

class Request {
 public:
  Request() : request_code_(),
              requester_sock_(),
              target_sock_(),
              is_derived_() {}
  ~Request() {}

  enum {
    kUnknown = 0,
    kPass,
    kNick,
    kUser,
    kPrivMsg,
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
  int getTargetSocket() const;
  void setRequestCode(int request_code);
  void setRequesterSocket(int requester_sock);
  void setTargetSocket(int target_sock);

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
  int request_code_;
  int requester_sock_;
  int target_sock_;

  std::string command_;
  std::string addi_;
  Vstring param_;

  bool is_derived_;
};

} // namespace irc

#endif // IRCSERV_REQUEST_H_
