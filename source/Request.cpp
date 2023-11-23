#include "Request.h"

namespace irc {

int getRequestCode() const { return request_code_; }

int getRequesterSocket() const { return requester_sock_; }

int getTargetSocket() const { return target_sock_; }

void setRequestCode(int request_code) { request_code_ = request_code; }

void setRequesterSocket(int requester_sock) {
  requester_sock_ = requester_sock;
}

void setTargetSocket(int target_sock) { target_sock_ = target_sock; }

std::string const &getCommand() const { return command_; }

std::string const &getAddi() const { return addi_; }

Vstring const &getParam() const { return param_; }

void setCommand(std::string const &command) { command_ = command; }

void setAddi(std::string const &addi) { addi_ = addi; }

void setParam(Vstring const &param) { param_ = param; }

void addParam(std::string const &param) { param_.push_back(param); }

bool isDerived() const { return is_derived_; }

void setDerived(bool is_derived) { is_derived_ = is_derived; }

} // namespace irc
