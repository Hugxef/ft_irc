/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 14:37:20 by thomathi          #+#    #+#             */
/*   Updated: 2023/07/28 18:27:20 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"

bool IRC::welcome_message(int i)
{
	std::string message;
	int			result;
	int			x;

	x = get_current_fd(i);
	if (this->_clients[x].welcome_done == 1)
		return false;

	message = RPL_WELCOME(user_id(this->_clients[x].nickname, this->_clients[x].username), this->_clients[x].nickname);
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}

	message = RPL_YOURHOST(user_id(this->_clients[x].nickname, this->_clients[x].username), "Reborn IRC", "0.3");
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}

	message = RPL_CREATED(this->_clients[x].nickname, "08/06/1999");
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}

	message = RPL_MYINFO(this->_clients[x].nickname, "Reborn IRC", "0.3", "io", "khost", "k");
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}

	message = RPL_ISUPPORT(this->_clients[x].nickname, "CHANNELLEN=32 NICKLEN=9 TOPICLEN=307");
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}

	this->_clients[x].welcome_done = 1;
	return false;
}

bool IRC::ping_pong(int i)
{
	std::string message;
	int			result;
	int			x;
	std::string token;

	x = get_current_fd(i);
	if (this->_last_cmd.size() < 2)
		return false;
	token = this->_last_cmd[1];
	message = RPL_PONG(user_id(this->_clients[x].nickname, this->_clients[x].username), token);
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}
	return false;
}

bool IRC::quit_irc(char *buffer, int i)
{
	std::string fd_nickname;
	std::string fd_username;
	std::string reason;
	std::string channel_test;
	int result;
	int x;

	x = get_current_fd(i);
	fd_nickname = this->_clients[x].nickname;
	fd_username = this->_clients[x].username;
	int z = 5;
	while (buffer[z] != '\n' && buffer[z] != '\0' && buffer[z] != '\r')
		reason.push_back(buffer[z++]);
	for (unsigned int j = 0; j < this->_clients[x].channels.size(); j++)
	{
		int channel_id = get_channel_id(this->_clients[x].channels[j]);
		if (channel_id == -1)
			continue ;
		this->_channels[channel_id].users_count--;
	}
	channel_test = RPL_QUIT(user_id(fd_nickname, this->_clients[x].username), reason);
	for (int j = 0; j < this->_amount_clients; j++)
	{
		result = send(this->_clients[j].fd, channel_test.c_str(), channel_test.length(), 0);
		if (result < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			return true;
		}
	}
	return true;
}

// Temp function
bool IRC::cap_ls_bs(int i)
{
	std::string message;
	int 		result;
	int			x;

	// Uniquement là pour les clients qui attendent une reponse pour les cap ls
	x = get_current_fd(i);
	message = RPL_WELCOME(user_id(this->_clients[x].nickname, this->_clients[x].username), this->_clients[x].nickname);
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}
	return false;
}

bool IRC::is_pass_correct(int i)
{
	int x;

	x = get_current_fd(i);
	if (this->_last_cmd[1] == this->_password)
	{
		this->_clients[x].pass_done = 1;
		return true;
	}
	return false;
}
