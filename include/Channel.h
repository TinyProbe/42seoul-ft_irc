#ifndef IRCSERV_CHANNEL_H_
#define IRCSERV_CHANNEL_H_

namespace irc {

typedef std::unordered_map<std::string, bool> Connection2;
typedef std::unordered_map<std::string, bool> BanList;
typedef std::unordered_map<std::string, bool> Operator;

class Channel {
 public:
  Channel() : invite_only_(),
              topic_only_(),
              has_password(),
              user_limit_(),
              limit_() {}
  ~Channel() {}

  std::string const &getName() const;
  void setName(std::string const &name);

  void ban(std::string const &nick);
  bool isBanned(std::string const &nick) const;

  Connection2 &getConnection2() const;
  bool isJoined(std::string const &nick) const;
  void disconnect(std::string const &nick);

  void setOrigin(std::string const &nick);
  void addOperator(std::string const &nick);
  void delOperator(std::string const &nick);
  int isOperator(std::string const &nick);

  bool getInviteOnly() const;
  bool getTopicOnly() const;
  bool getHasPassword() const;
  bool getUserLimit() const;
  void setInviteOnly(bool invite_only);
  void setTopicOnly(bool topic_only);
  void setHasPassword(bool has_password);
  void setUserLimit(bool user_limit);

  bool verify(std::string const &password) const;
  void setPassword(std::string const &password);
  size_t getLimit() const;
  void setLimit(size_t limit);

 private:
  std::string name_;

  Connection2 connection2_;
  BanList     ban_list_;

  std::string origin_;
  Operator    operator_;

  bool        invite_only_;
  bool        topic_only_;
  bool        has_password_;
  bool        user_limit_;
  std::string password_;
  size_t      limit_;
};

} // namespace irc

#endif // IRCSERV_CHANNEL_H_
