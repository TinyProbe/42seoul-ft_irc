/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migo <migo@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:34 by migo              #+#    #+#             */
/*   Updated: 2023/11/29 22:17:34 by migo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.h"

#include "common.h"

namespace irc {

Request &Request::operator=(Request const &rhs) {
  if (this == &rhs) { return *this; }
  request_code_   = rhs.request_code_;
  requester_sock_ = rhs.requester_sock_;
  target_         = rhs.target_;
  command_        = rhs.command_;
  addi_           = rhs.addi_;
  param_          = rhs.param_;
  is_derived_     = rhs.is_derived_;
  return *this;
}

int Request::getRequestCode() const { return request_code_; }

int Request::getRequesterSocket() const { return requester_sock_; }

std::string const &Request::getTarget() const { return target_; }

void Request::setRequestCode(int request_code) { request_code_ = request_code; }

void Request::setRequesterSocket(int requester_sock) {
  requester_sock_ = requester_sock;
}

void Request::setTarget(std::string const &target) { target_ = target; }

std::string const &Request::getCommand() const { return command_; }

std::string const &Request::getAddi() const { return addi_; }

Vstring const &Request::getParam() const { return param_; }

void Request::setCommand(std::string const &command) { command_ = command; }

void Request::setAddi(std::string const &addi) { addi_ = addi; }

void Request::setParam(Vstring const &param) { param_ = param; }

void Request::addParam(std::string const &param) { param_.push_back(param); }

bool Request::isDerived() const { return is_derived_; }

void Request::setDerived(bool is_derived) { is_derived_ = is_derived; }

} // namespace irc
