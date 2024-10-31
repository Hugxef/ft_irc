/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Operators.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 13:45:52 by thomathi          #+#    #+#             */
/*   Updated: 2023/08/01 10:20:36 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"

bool IRC::kick(int i)
{
	int ope_pos;
	std::string ope_nickname;
	std::string ope_username;
	std::string token;

	ope_pos = get_current_fd(i);
	ope_nickname = this->_clients[ope_pos].nickname;
	ope_username = this->_clients[ope_pos].username;
	if (this->_last_cmd.size() < 4)
	{
		std::cout << "Error: kick() failed 1" << std::endl;
		return false;
	}
	if (this->_last_cmd[1].c_str()[0] != '#')
	{
		std::cout << "Error: kick() failed 2" << std::endl;
		return false;
	}
	if (!check_is_in_channel(ope_pos, this->_last_cmd[1]))
	{
		std::cout << "Error: kick() failed 3" << std::endl;
		return false;
	}
	if (!is_operator(ope_pos, this->_last_cmd[1]))
	{
		std::cout << "Error: kick() failed 4" << std::endl;
		return false;
	}
	if (this->_last_cmd[2] == ope_nickname)
	{
		std::cout << "Error: kick() failed 5" << std::endl;
		return false;
	}
	for (unsigned int x = 0; x < this->_last_cmd[3].length(); x++)
	{
		if (this->_last_cmd[3][x] == ':')
		{
			while (this->_last_cmd[3][x] != '\n' && this->_last_cmd[3][x] != '\0' && this->_last_cmd[3][x] != '\r')
			{
				token.push_back(this->_last_cmd[3][x]);
				x++;
			}
		}
		for (unsigned int y = 4; y < this->_last_cmd.size(); y++)
		{
			token.push_back(' ');
			token.append(this->_last_cmd[y]);
		}
	}
	std::string message = RPL_KICK(user_id(ope_nickname, ope_username), this->_last_cmd[1], this->_last_cmd[2], token);
	for (int x = 0; x < this->_amount_clients; x++)
	{
		if (send(this->_clients[x].fd, message.c_str(), message.length(), 0) < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			return true;
		}
		if (this->_clients[x].nickname == this->_last_cmd[2])
		{
			for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
			{
				if (this->_clients[x].channels[z] == this->_last_cmd[1])
				{
					this->_clients[x].channels.erase(this->_clients[x].channels.begin() + z);
					this->_clients[x].mode_channel.erase(this->_clients[x].mode_channel.begin() + z);
					this->_channels[get_channel_id(this->_last_cmd[1])].users_count--;
					break;
				}
			}
			break;
		}
		else 
		{
			std::string message = ERR_USERNOTINCHANNEL(ope_username, ope_nickname, this->_last_cmd[1]);
			if(send(this->_clients[get_current_fd(i)].fd, message.c_str(), message.size(), 0) == -1)
			return false;
		}
	}
	return false;
}

bool IRC::topic(char *buffer, int i)
{
	int x;
	std::string message = "";
	int y = 0;

	x = get_current_fd(i);
	if (this->_last_cmd.size() < 3)
	{
		std::cout << "Error: topic() failed 1" << std::endl;
		return false;
	}
	if (this->_last_cmd[1].c_str()[0] != '#')
	{
		std::cout << "Error: topic() failed 2" << std::endl;
		return false;
	}
	if (!check_is_in_channel(x, this->_last_cmd[1]))
	{
		std::cout << "Error: topic() failed 3" << std::endl;
		return false;
	}
	if (this->_clients[x].mode_channel[get_channel_id(this->_last_cmd[1])].find('o') == std::string::npos)
	{
		std::cout << "Error: topic() failed 4" << std::endl;
		return false;
	}
	if (this->_last_cmd[2].c_str()[0] == ':')
	{
		while (buffer[y] != ':')
			y++;
		this->_channels[get_channel_id(this->_last_cmd[1])].topic = std::string(&buffer[y + 1]);
		message = RPL_TOPIC(user_id(this->_clients[x].nickname, this->_clients[x].username), this->_last_cmd[1], this->_channels[get_channel_id(this->_last_cmd[1])].topic);
		this->_channels[get_channel_id(this->_last_cmd[1])].rpl_topic = message;
		for (int y = 0; y < this->_amount_clients; y++)
		{
			if (send(this->_clients[y].fd, message.c_str(), message.length(), 0) < 0)
			{
				std::cout << "Error: send() failed" << std::endl;
				return true;
			}
		}
	}
	else
	{
		std::cout << "Error: topic() failed 4" << std::endl;
		return false;
	}
	return false;
}
