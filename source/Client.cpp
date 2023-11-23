#include "Client.h"

namespace irc {

static int const Client::kMaxChannel = 50;
static int const kMaxBuffer = (1 << 20);

static Request makeRequest_(int request_code,
                            int requester_sock,
                            int target_sock,
                            std::string const &command,
                            std::string const &addi,
                            Vstring const &param,
                            bool is_derived) {
  Request req;
  req.setRequestCode(request_code);
  req.setRequesterSocket(requester_sock);
  req.setTargetSocket(target_sock);
  req.setCommand(command);
  req.setAddi(addi);
  req.setParam(param);
  req.setDerived(is_derived);
  return req;
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

std::string Server::getIdentify() const {
  return nick_ + "!" + user_ + "@" + host_;
}

std::string &Client::getBuffer() { return buffer_; }

bool Client::canWrite() const { return can_write_; }

void Client::setWrite(bool can_write) { can_write_ = can_write; }

UMstring_bool const &Client::getJoinedChannel() const {
  return joined_channel_;
}

void Client::join(std::string const &channel) {
  joined_channel_[channel] = true;
}

void Client::part(std::string const &channel) {
  joined_channel_.erase(channel);
}

bool Client::isJoined(std::string const &channel) const {
  if (joined_channel_.find(channel) != joined_channel_.end()) {
    return true;
  }
  return false;
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
  if (buffer_.size() == 0) { return false; }
  // ... <- here!!!
  request_ = makeRequest_();
  return true;
}

Request const &Client::getRequest() const { return request_; }

} // namespace irc
