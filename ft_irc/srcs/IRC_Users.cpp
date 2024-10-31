/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Users.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 14:38:11 by thomathi          #+#    #+#             */
/*   Updated: 2023/07/28 01:22:55 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

void IRC::change_nickname(char *buffer, int i)
{
	int x;
	int j = 5;
	std::string new_name;
	std::string message;

	std::cout << "Name changed ask for : " << this->_pollfd[i].fd << std::endl;
	x = get_current_fd(i);
	while (buffer[j] != '\n' && buffer[j] != '\r' && buffer[j] != '\0')
	{
		new_name.push_back(buffer[j]);
		j++;
	}
	for (int z = 0; z < this->_amount_clients; z++)
	{
		if (this->_clients[z].nickname == new_name)
		{
			if (z != x)
				new_name = new_name + "_";
			break;
		}
	}
	for (unsigned int z = 0; z < this->_clients.size(); z++)
	{
		message = ":" + this->_clients[x].nickname + "!~" + this->_clients[x].username + "@localhost NICK " + new_name + "\r\n";
		int result = send(this->_clients[z].fd, message.c_str(), message.length(), 0);
		if (result < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			return ;
		}
	}
	this->_clients[x].nickname = new_name;
}

int IRC::get_current_fd(int i)
{
	for (int x = 0; x < this->_amount_clients; x++)
	{
		if (this->_clients[x].fd == this->_pollfd[i].fd)
			return x;
	}
	return i;
}

void IRC::change_username(int i)
{
	int is_registered = 0;
	for (unsigned int x = 0; x < this->_clients.size(); x++)
	{
		if (this->_clients[x].username == this->_last_cmd[1])
		{
			std::string message = ERR_ALREADYREGISTERED(this->_clients[x].nickname);
			if (send(this->_clients[get_current_fd(i)].fd, message.c_str(), message.size(), 0) == -1)
			{
				std::cout << "Error: send() failed" << std::endl;
			}
			is_registered = 1;
		}
	}
	if (is_registered == 0)
		this->_clients[get_current_fd(i)].username = this->_last_cmd[1];
	return ;
}
