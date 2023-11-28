#include "common.h"

namespace irc {

int const Client::kMaxChannel = 50;
static int const kMaxBuffer = (1 << 20);

static Request makeRequest_(int request_code,
                            int requester_sock,
                            std::string const &target,
                            std::string const &command,
                            std::string const &addi,
                            Vstring const &param,
                            bool is_derived) {
  Request req;
  req.setRequestCode(request_code);
  req.setRequesterSocket(requester_sock);
  req.setTarget(target);
  req.setCommand(command);
  req.setAddi(addi);
  req.setParam(param);
  req.setDerived(is_derived);
  return req;
}

static bool parse(std::string &buffer,
                  std::string &command,
                  Vstring &param) {
  size_t p = buffer.find("\r\n");
  if (p == std::string::npos) { return false; }
  std::string msg = buffer.substr(0, p);
  p += 2;
  buffer = buffer.substr(p, buffer.size() - p);

#ifdef _DEBUG_
  std::cout << msg << std::endl;
#endif

  std::stringstream ss;
  std::string word;
  ss << msg;
  ss >> word;
  command = word;
  while (ss >> word) {
    if (word[0] == ':') {
      size_t p2 = msg.find(':');
      word = msg.substr(p2, msg.size() - p2);
      param.push_back(word);
      break;
    }
    param.push_back(word);
  }
  return true;
}

bool Client::getAuth() const { return auth_; }

std::string const &Client::getPassword() const { return password_; }

int Client::getSocket() const { return sock_; }

struct sockaddr_in const &Client::getAddress() const { return addr_; }

void Client::setAuth(bool auth) { auth_ = auth; }

void Client::setPassword(std::string const &password) { password_ = password; }

void Client::setSocket(int sock) { sock_ = sock; }

void Client::setAddress(struct sockaddr_in const &addr) {
  addr_ = addr;
  int ip = ntohl(addr.sin_addr.s_addr);
  int port = ntohs(addr.sin_port);
  host_  = std::to_string(((unsigned char *) &ip)[3]) + '.';
  host_ += std::to_string(((unsigned char *) &ip)[2]) + '.';
  host_ += std::to_string(((unsigned char *) &ip)[1]) + '.';
  host_ += std::to_string(((unsigned char *) &ip)[0]) + ':';
  host_ += std::to_string(port);
}

std::string const &Client::getNick() const { return nick_; }

std::string const &Client::getUser() const { return user_; }

std::string const &Client::getReal() const { return real_; }

std::string const &Client::getHost() const { return host_; }

void Client::setNick(std::string const &nick) { nick_ = nick; }

void Client::setUser(std::string const &user) { user_ = user; }

void Client::setReal(std::string const &real) { real_ = real; }

void Client::setHost(std::string const &host) { host_ = host; }

std::string Client::getIdentify() const {
  return nick_ + "!" + user_ + "@" + host_;
}

std::string &Client::getBuffer() { return buffer_; }

bool Client::canWrite() const { return can_write_; }

void Client::setWrite(bool can_write) { can_write_ = can_write; }

UMstring_bool &Client::getJoinedChannel() { return joined_channel_; }

void Client::join(std::string const &channel) {
  joined_channel_[channel] = true;
}

void Client::part(std::string const &channel) {
  joined_channel_.erase(channel);
}

bool Client::isJoined(std::string const &channel) const {
  return joined_channel_.find(channel) != joined_channel_.end();
}

bool Client::receive() {
  static char buf[kMaxBuffer + 1];
  static int len;
  buffer_.clear();
  while (true) {
    len = recv(sock_, buf, kMaxBuffer, MSG_DONTWAIT | MSG_NOSIGNAL);
    if (len == -1 && errno != EAGAIN) {
      std::cerr << std::string("recv: ") +
                   std::string(strerror(errno)) << std::endl;
      return false;
    }
    buf[std::max(len, 0)] = '\0';
    buffer_ += buf;
    if (len < kMaxBuffer) { return true; }
  }
}

bool Client::makeRequest() {
  int request_code = 0;
  int requester_sock = sock_;
  std::string command;
  std::string target;
  std::string addi;
  Vstring param;
  if (!parse(buffer_, command, param)) { return false; }
  if (command == "PASS") {
    request_code = Request::kPass;
  } else if (command == "NICK") {
    request_code = Request::kNick;
    if (param.size() >= 1 && param[0][0] == ':') {
      param.resize(0);
    }
  } else if (command == "USER") {
    request_code = Request::kUser;
  } else if (command == "PRIVMSG") {
    request_code = Request::kPrivMsg;
    if (param.size() >= 1) {
      target = param[0];
    }
  } else if (command == "JOIN") {
    request_code = Request::kJoin;
  } else if (command == "NAMES") {
    request_code = Request::kNames;
  } else if (command == "PART") {
    request_code = Request::kPart;
    if (param.size() >= 2) {
      addi = param[1];
      param.resize(1);
    } else if (param.size() >= 1 && param[0][0] == ':') {
      param.resize(0);
    }
  } else if (command == "KICK") {
    request_code = Request::kKick;
    if (param.size() >= 3) {
      addi = param[2];
      param.resize(2);
    }
  } else if (command == "INVITE") {
    request_code = Request::kInvite;
    target = param[0];
  } else if (command == "ACCEPT") {
    request_code = Request::kAccept;
  } else if (command == "DENY") {
    request_code = Request::kDeny;
  } else if (command == "TOPIC") {
    request_code = Request::kTopic;
    if (param.size() >= 2) {
      addi = param[1];
      param.resize(1);
    } else if (param.size() >= 1 && param[0][0] == ':') {
      param.resize(0);
    }
  } else if (command == "MODE") {
    request_code = Request::kMode;
  } else {
    request_code = Request::kUnknown;
  }
  request_ = makeRequest_(request_code,
                          requester_sock,
                          target, command, addi, param, false);
  return true;
}

Request const &Client::getRequest() const { return request_; }

} // namespace irc
