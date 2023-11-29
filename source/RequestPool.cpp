/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestPool.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migo <migo@student.42seoul.kr>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:46 by migo              #+#    #+#             */
/*   Updated: 2023/11/29 22:17:46 by migo             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestPool.h"

#include "common.h"

namespace irc {

void RequestPool::push(Request const &req) { new_requests_.push(req); }

bool RequestPool::pollRequest(Request &req) {
  if (new_requests_.empty()) {
    return false;
  }
  req = new_requests_.front();
  new_requests_.pop();
  return true;
}

size_t RequestPool::getSize() const { return new_requests_.size(); }

} // namespace irc
