/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Debug.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 14:32:45 by thomathi          #+#    #+#             */
/*   Updated: 2023/06/21 14:33:29 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

void IRC::debug_infos()
{
	for (unsigned x = 0; x < this->_clients.size(); x++)
	{
		std::cout << "---------" << x << "--" << std::endl;
		std::cout << "Nickname : " << this->_clients[x].nickname << std::endl;
		std::cout << "Username : " << this->_clients[x].username << std::endl;
		std::cout << "FD       : " << this->_clients[x].fd << std::endl;
	}
}

void IRC::print_debug(char *buffer)
{
	std::cout << "------------------------DEBUG-----" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "------------------------DEBUG-----" << std::endl;
}

