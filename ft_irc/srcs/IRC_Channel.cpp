/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Channel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 14:34:48 by thomathi          #+#    #+#             */
/*   Updated: 2023/08/01 20:40:18 by hugrene          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"

std::string check_is_channel(std::string hashtag)
{
	std::string channel;
	if (hashtag[0] == '#')
		return "";
	return hashtag;
}

bool IRC::join_channel(int i)
{
	std::string	message;
	std::string	fd_nickname;
	std::string	fd_username;
	std::string	channel;
	std::string	channel_test;
	int			x;
	int			result;
	bool		close_conn = false;

	x = get_current_fd(i);
	channel = "";
	fd_nickname = this->_clients[x].nickname;
	fd_username = this->_clients[x].username;
	for (unsigned int z = 0; z < this->_last_cmd.size(); z++)
	{
		if (this->_last_cmd[z][0] == '#')
		{
			channel = this->_last_cmd[z];
			break;
		}
	}
	if (channel.empty())
		return close_conn;
	if (avoid_dup_channel(i, channel))
		return close_conn;
	message = RPL_JOIN(user_id(fd_nickname, this->_clients[x].username), channel);
	for (unsigned int z = 0; z < this->_clients.size(); z++)
	{
		for (unsigned int y = 0; y < this->_clients[z].channels.size(); y++)
		{
			if (this->_clients[z].channels[y] == channel)
			{
				result = send(this->_clients[z].fd, message.c_str(), message.length(), 0);
				if (result < 0)
				{
					std::cout << "Error: send() failed" << std::endl;
					close_conn = true;
					break;
				}
				break;
			}
		}
	}
	result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
	if (result < 0)
	{
		std::cout << "Error: send() failed" << std::endl;
		close_conn = true;
	}
	if (is_new_channel(channel))
	{
		this->_channels.push_back(Channel());
		this->_channels[this->_amount_channels].name = channel;
		this->_channels[this->_amount_channels].users_count = 1;
		this->_amount_channels++;
		message = RPL_CHANNELMODEIS(user_id(fd_nickname, this->_clients[x].username), channel, "+o");
		result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
		if (result < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			close_conn = true;
		}
		this->_clients[x].mode_channel.push_back("o");
	}
	else
	{
		int channel_id = get_channel_id(channel);
		if (channel_id == -1)
			return close_conn;
		this->_channels[channel_id].users_count++;
		this->_clients[x].mode_channel.push_back("v");
	}
	if (this->_channels[get_channel_id(channel)].topic.empty())
	{
		message = RPL_NOTOPIC(user_id(fd_nickname, this->_clients[x].username), channel);
		result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
		if (result < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			close_conn = true;
		}
	}
	else
	{
		message = this->_channels[get_channel_id(channel)].rpl_topic;
		result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
		if (result < 0)
		{
			std::cout << "Error: send() failed" << std::endl;
			close_conn = true;
		}
	}
	this->_clients[x].channels.push_back(channel);
	who_in_channel(i);
	return close_conn;
}

bool IRC::part_channel(int i)
{
	std::string fd_nickname, fd_username, buffer, name, channel, channel_test;
	std::string reason = "Leaving";
	int result;
	int x;
	bool close_conn = false;

	x = get_current_fd(i);
	channel = "";
	fd_nickname = this->_clients[x].nickname;
	fd_username = this->_clients[x].username;
	for (unsigned int z = 0; z < this->_last_cmd.size(); z++)
	{
		if (this->_last_cmd[z][0] == '#')
		{
			channel = this->_last_cmd[z];
			break;
		}
	}
	for (unsigned int z = 0; z < this->_last_cmd.size(); z++)
	{
		if (this->_last_cmd[z][0] == ':')
		{
			reason = "";
			while (z < this->_last_cmd.size())
			{
				reason.append(this->_last_cmd[z]);
				reason.push_back(' ');
				z++;
			}
			break;
		}
	}
	if (!check_is_in_channel(i, channel))
	{
		std::cout << "Not in channel : " << channel << std::endl;
		return close_conn;
	}
	channel_test = RPL_PART(user_id(fd_nickname, this->_clients[x].username), channel, reason);
	for (int j = 0; j < this->_amount_clients; j++)
	{
		for (unsigned int k = 0; k < this->_clients[j].channels.size(); k++)
		{
			if (this->_clients[j].channels[k] == channel)
			{
				result = send(this->_clients[j].fd, channel_test.c_str(), channel_test.length(), 0);
				if (result < 0)
				{
					std::cout << "Error: send() failed" << std::endl;
					close_conn = true;
					break;
				}
				break;
			}
		}
	}
	int channel_id = get_channel_id(channel);
	if (channel_id == -1)
		return close_conn;
	this->_channels[channel_id].users_count--;
	for (unsigned int j = 0; j < this->_clients[x].channels.size(); j++)
	{
		if (this->_clients[x].channels[j] == channel)
		{
			this->_clients[x].channels.erase(this->_clients[x].channels.begin() + j);
			this->_clients[x].mode_channel.erase(this->_clients[x].mode_channel.begin() + j);
			break;
		}
	}
	return close_conn;
}

bool IRC::who_in_channel(int i)
{
	bool		close_conn = false;
	std::string	message;
	std::string user_list;
	int			result;
	int			x;

	x = get_current_fd(i);
	user_list = get_users_in_channel(this->_last_cmd[1]);
	message = RPL_NAMREPLY(this->_clients[x].nickname, "=", this->_last_cmd[1], user_list);
	result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
	if (result < 0)
	{
		std::cout << "error: send() failed" << std::endl;
		close_conn = true;
	}
	message = ":localhost 366 ";
	message.append(this->_clients[x].nickname + " ");
	message.append(this->_last_cmd[1]);
	message.append(" :END of NAMES list");
	message.append("\r\n");
	result = send(this->_clients[x].fd, message.c_str(), message.length(), 0);
	if (result < 0)
	{
		std::cout << "error: send() failed" << std::endl;
		close_conn = true;
	}
	return close_conn;
}

std::string IRC::get_users_in_channel(std::string channel)
{
	std::string users = "";
	for (int x = 0; x < this->_amount_clients; x++)
	{
		for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
		{
			if (this->_clients[x].channels[z].compare(channel) == 0)
			{
				if (this->_clients[x].mode_channel[z].find('o') != std::string::npos)
					users.append("@" + this->_clients[x].nickname + " ");
				else
					users.append(this->_clients[x].nickname + " ");
			}
		}
	}
	return users;
}

int IRC::is_new_channel(std::string channel)
{
	for (unsigned int z = 0; z < this->_channels.size(); z++)
	{
		if (this->_channels[z].name == channel && this->_channels[z].users_count > 0)
			return false;
	}
	return true;
}

bool IRC::check_is_in_channel(int i, std::string channel)
{
	int x = get_current_fd(i);
	for (unsigned int j = 0; j < this->_clients[x].channels.size(); j++)
	{
		if (this->_clients[x].channels[j].compare(channel) == 0)
			return true;
	}
	return false;
}

bool IRC::notice(char *buffer, int i)
{
	int x;
	int y = 0;
	std::string token;
	
	x = get_current_fd(i);
	if (this->_last_cmd.size() < 3)
		return false;
	if (this->_last_cmd[1].c_str()[0] == '#')
	{
		if (!check_is_in_channel(i, this->_last_cmd[1]))
		{
			std::cout << "Not in channel : " << this->_last_cmd[1] << std::endl;
			return false;
		}
		if (this->_clients[x].mode_channel[get_channel_id(this->_last_cmd[1])].find('o') == std::string::npos)
		{
			std::cout << "Cannot send notice to channel : " << this->_last_cmd[1] << std::endl;
			return false;
		}
		if (this->_last_cmd[2].c_str()[0] == ':')
		{
			while (buffer[y] != ':')
				y++;
		}
		else
			token = this->_last_cmd[2];
		token = std::string(&buffer[y]);
		if (token.find('\n') != token.npos)
			token.erase(token.find('\n'), 1);
		if (token.find('\r') != token.npos)
			token.erase(token.find('\r'), 1);
		for (int j = 0; j < this->_amount_clients; j++)
		{
			for (unsigned int k = 0; k < this->_clients[j].channels.size(); k++)
			{
				if (this->_clients[j].channels[k] == this->_last_cmd[1] && this->_clients[j].nickname != this->_clients[x].nickname)
				{
					std::string message = RPL_NOTICE(this->_clients[x].nickname, this->_clients[x].username, this->_last_cmd[1], token);
					int result = send(this->_clients[j].fd, message.c_str(), message.length(), 0);
					if (result < 0)
					{
						std::cout << "Error: send() failed" << std::endl;
						return true;
					}
					break;
				}
			}
		}
	}
	else
	{
		if (this->_last_cmd[2].c_str()[0] == ':')
		{
			while (buffer[y] != ':')
				y++;
		}
		else
			token = this->_last_cmd[2];
		token = std::string(&buffer[y]);
		if (token.find('\n') != token.npos)
			token.erase(token.find('\n'), 1);
		if (token.find('\r') != token.npos)
			token.erase(token.find('\r'), 1);
		for (int j = 0; j < this->_amount_clients; j++)
		{
			if (this->_clients[j].nickname == this->_last_cmd[1])
			{
				std::string message = RPL_NOTICE(this->_clients[x].nickname, this->_clients[x].username, this->_last_cmd[1], token);
				int result = send(this->_clients[j].fd, message.c_str(), message.length(), 0);
				if (result < 0)
				{
					std::cout << "Error: send() failed" << std::endl;
					return true;
				}
				break;
			}
		}
	}
	return false;
}

bool IRC::priv_message(char *buffer, int i)
{
	int x;
	int y = 0;
	int hashtag;
	bool for_channel = false;
	bool close_conn = false;
	std::string message;
	std::string token;

	x = get_current_fd(i);
	hashtag = this->_last_cmd[1].find('#');
	if (hashtag == 0)
	{
		for_channel = true;
		if (!check_is_in_channel(i, this->_last_cmd[1]))
		{
			std::cout << "Not in channel : " << this->_last_cmd[1] << std::endl;
			return close_conn;
		}
	}
	while (buffer[y] != ':')
		y++;
	token = std::string(&buffer[y + 1]);
	message = RPL_PRIVMSG(this->_clients[x].nickname, this->_clients[x].username, this->_last_cmd[1], token);
	if (for_channel)
	{
		for (unsigned int z = 0; z < this->_clients.size(); z++)
		{
			for (unsigned int w = 0; w < this->_clients[z].channels.size(); w++)
			{
				if (this->_clients[z].nickname == this->_clients[x].nickname)
					continue;
				if (this->_clients[z].channels[w] == this->_last_cmd[1])
				{
					int result = send(this->_clients[z].fd, message.c_str(), message.length(), 0);
					if (result < 0)
					{
						std::cout << "Error: send() failed" << std::endl;
						close_conn = true;
						break;
					}
				}
			}
		}
	}
	else
	{
		for (unsigned int z = 0; z < this->_clients.size(); z++)
		{
			if (this->_clients[z].nickname == this->_last_cmd[1])
			{
				int result = send(this->_clients[z].fd, message.c_str(), message.length(), 0);
				if (result < 0)
				{
					std::cout << "Error: send() failed" << std::endl;
					close_conn = true;
					break;
				}
				break;
			}
		}
	}
	return close_conn;
}

// TODO: dégager ça si ça fonctionne le nouvelle fonction
/**
bool IRC::priv_message(char *buffer, int i)
{
	std::string message;
	int result;
	int x;
	int y = 0;
	bool is_in_channel = false;
	bool close_conn = false;
	std::string channel_args;

	x = get_current_fd(i);
	channel_args = check_is_channel(this->_last_cmd[1]);
	is_in_channel = check_is_in_channel(i, channel_args);
	if (channel_args != "")
	{
		if (is_in_channel == false)
		{
			std::cout << "Not in channel : " << channel_args << std::endl;
			return close_conn;
		}
	}
	while (buffer[y] != ':')
		y++;
	while (buffer[y] != '\n' && buffer[y] != '\r')
	{
		message.push_back(buffer[y]);
		y++;
	}
	message = RPL_PRIVMSG(this->_clients[x].nickname, this->_clients[x].username, this->_last_cmd[1], message);
	for (int j = 0; j < this->_amount_clients; j++)
	{
		if (channel_args == "")
		{
			if (this->_clients[j].fd == this->_pollfd[i].fd)
				continue;
			result = send(this->_clients[j].fd, message.c_str(), message.length(), 0);
			if (result < 0)
			{
				std::cout << "Error: send() failed" << std::endl;
				close_conn = true;
				break;
			}
		}
		else
		{
			if (this->_clients[j].nickname == channel_args)
			{
				result = send(this->_clients[j].fd, message.c_str(), message.length(), 0);
				if (result < 0)
				{
					std::cout << "Error: send() failed" << std::endl;
					close_conn = true;
					break;
				}
			}
			else
				continue;
		}
	}
	return close_conn;
}
**/

bool IRC::avoid_dup_channel(int i, std::string channel)
{
	int x;

	x = get_current_fd(i);
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		if (this->_clients[x].channels[z] == channel)
			return true;
	}
	return false;
}

int IRC::get_channel_id(std::string channel)
{
	for (int i = 0; i < this->_amount_channels; i++)
	{
		if (this->_channels[i].name == channel)
			return i;
	}
	return -1;
}

bool IRC::is_operator(int i, std::string channel)
{
	int x;

	x = get_current_fd(i);
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		if (this->_clients[x].channels[z] == channel)
		{
			if (this->_clients[x].mode_channel[z].find("o") != std::string::npos)
				return true;
			else
				return false;
		}
	}
	return false;
}

