/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkong <tkong@student.42seoul.kr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/29 22:17:38 by tkong             #+#    #+#             */
/*   Updated: 2023/11/29 22:17:38 by tkong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

#include <signal.h>

#include <iostream>

namespace irc {

void sigpipe(int sig) {
  if (sig == SIGPIPE) {
    std::cerr << "SIGPIPE occured" << std::endl;
  }
}

} // namespace irc
