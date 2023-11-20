#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

typedef std::unordered_map<std::string, bool> Joined;
typedef std::unordered_map<std::string, bool> BanList;
typedef std::unordered_map<std::string, bool> Operator;

class Channel {
 public:
  Channel() : invite_only_(),
              oper_topic_(),
              has_password(),
              user_limit_(),
              limit_() {}
  ~Channel() {}

  std::string const &getName() const;
  void setName(std::string const &name);

  void ban(std::string const &nick);
  bool isBanned(std::string const &nick) const;

  Joined &getJoined() const;
  bool isJoined(std::string const &nick) const;
  bool join(std::string const &nick);
  void part(std::string const &nick);

  void setOrigin(std::string const &nick);
  void addOperator(std::string const &nick);
  void delOperator(std::string const &nick);
  int isOperator(std::string const &nick);

  bool getInviteOnly() const;
  bool getOperTopic() const;
  bool getHasPassword() const;
  bool getUserLimit() const;
  void setInviteOnly(bool invite_only);
  void setOperTopic(bool oper_topic);
  void setHasPassword(bool has_password);
  void setUserLimit(bool user_limit);

  bool verify(std::string const &password) const;
  void setPassword(std::string const &password);
  size_t getLimit() const;
  void setLimit(size_t limit);

 private:
  std::string name_;

  Joined  joined_;
  BanList ban_list_;

  std::string origin_;
  Operator    operator_;

  bool        invite_only_;
  bool        oper_topic_;
  bool        has_password_;
  bool        user_limit_;
  std::string password_;
  size_t      limit_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_
