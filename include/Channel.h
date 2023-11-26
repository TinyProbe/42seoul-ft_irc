#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

#include <unordered_map>
#include <string>

namespace irc {

typedef std::unordered_map<std::string, bool> UMstring_bool;

class Channel {
 public:
  Channel() : invite_only_(),
              oper_topic_(),
              has_password_(),
              has_limit_(),
              limit_() {}
  ~Channel() {}

  std::string const &getName() const;
  void setName(std::string const &name);

  void addInvite(std::string const &nick);
  void delInvite(std::string const &nick);
  bool isInvited(std::string const &nick) const;

  UMstring_bool &getJoinedClient();
  bool isJoined(std::string const &nick) const;
  void join(std::string const &nick);
  void part(std::string const &nick);

  void setOrigin(std::string const &nick);
  void addOperator(std::string const &nick);
  void delOperator(std::string const &nick);
  int isOperator(std::string const &nick) const;

  bool getInviteOnly() const;
  bool getOperTopic() const;
  bool getHasPassword() const;
  bool getHasLimit() const;
  void setInviteOnly(bool invite_only);
  void setOperTopic(bool oper_topic);
  void setHasPassword(bool has_password);
  void setHasLimit(bool has_limit);

  std::string const &getTopic() const;
  bool verify(std::string const &password) const;
  size_t getLimit() const;
  void setTopic(std::string const &topic);
  void setPassword(std::string const &password);
  void setLimit(size_t limit);

 private:
  std::string name_;

  UMstring_bool joined_client_;
  UMstring_bool invite_list_;

  std::string   origin_;
  UMstring_bool operator_;

  bool        invite_only_;
  bool        oper_topic_;
  bool        has_password_;
  bool        has_limit_;
  std::string topic_;
  std::string password_;
  size_t      limit_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_
