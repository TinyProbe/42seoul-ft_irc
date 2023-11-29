/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:39 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:39 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_COMMON_H_
#define IRCSERV_COMMON_H_

#include <string>
#include <sstream>

namespace irc {

template<typename T> T string_to(std::string const &s) {
  std::stringstream ss;
  T res;
  ss << s;
  ss >> res;
  return res;
}

void sigpipe(int sig);

} // namespace irc

#endif // IRCSERV_COMMON_H_
