/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Modes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/27 11:36:08 by thomathi          #+#    #+#             */
/*   Updated: 2023/07/31 21:12:27 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"

void IRC::set_new_modes(std::string elem, int user_id, int chan_id)
{
	std::string removed;
	for (unsigned int i = 0; i < elem.size(); i++)
	{
		if (this->_clients[user_id].mode_channel[chan_id].find(elem[i]) == std::string::npos && removed.find(elem[i]) == std::string::npos)
			this->_clients[user_id].mode_channel[chan_id].push_back(elem[i]);
		else
		{
			removed.push_back(elem[i]);
			this->_clients[user_id].mode_channel[chan_id].erase(this->_clients[user_id].mode_channel[chan_id].find(elem[i]), 1);
		}
	}
}

bool IRC::channel_mode(int i)
{
	int x;
	std::string modes;
	std::string message;

	x = get_current_fd(i);
	if (this->_clients[x].channels.size() == 0)
		return false;
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		for (unsigned int y = 0; y < this->_channels.size(); y++)
		{
			if (this->_clients[x].channels[z] == this->_channels[y].name)
				modes = this->_clients[x].mode_channel[z];
		}
	}
	if (modes == "")
		return false;
	message = RPL_CHANNELMODEIS(this->_clients[x].nickname, this->_last_cmd[1], modes);
	if(send(this->_clients[x].fd, message.c_str(), message.size(), 0) == -1)
		return true;
	return false;
}

bool IRC::channel_mode_args(int i)
{
	int x;

	x = get_current_fd(i);

	if (this->_last_cmd.size() < 3)
		return channel_mode(i);
	if (this->_clients[x].channels.size() == 0)
		return false;
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		for (unsigned int y = 0; y < this->_channels.size(); y++)
		{
			if (this->_clients[x].channels[z] == this->_channels[y].name)
				set_new_modes(this->_last_cmd[2], x, z);
		}
	}
	return channel_mode(i);
}

/**
bool IRC::channel_mode(int i)
{
	int x;
	int in_channel;
	std::string message;

	x = get_current_fd(i);
	if (this->_clients[x].channels.size() == 0)
		return false;
	in_channel = 0;
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		for (unsigned int y = 0; y < this->_channels.size(); y++)
		{
			if (this->_clients[x].channels[z] == this->_channels[y].name)
			{
				if (this->_clients[x].operator_channels[z] == 1)
					in_channel = 1;
				else
					in_channel = 2;
				break;
			}
		}
		if (in_channel == 0)
		{
			message = ERR_CHANOPERISNEEDED(this->_clients[x].nickname, this->_last_cmd[1]);
			if(send(this->_clients[x].fd, message.c_str(), message.size(), 0) == -1)
				return false;
		}
	}
	if (in_channel == 1)
		message = RPL_CHANNELMODEIS(this->_clients[x].nickname, this->_last_cmd[1], "+nt");
	else if (in_channel == 2)
		message = RPL_CHANNELMODEIS(this->_clients[x].nickname, this->_last_cmd[1], "+n");
	if (send(this->_clients[x].fd, message.c_str(), message.size(), 0) == -1)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}
	return false;
}
**/

bool IRC::user_mode(int i)
{
	int x;

	x = get_current_fd(i);
	std::string message = RPL_UMODEIS(this->_clients[x].nickname, "+i");
	if (send(this->_clients[x].fd, message.c_str(), message.size(), 0) == -1)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}
	return false;
}

/**
bool IRC::channel_mode_args(int i)
{
	int x;
	int in_channel;
	std::string message;

	x = get_current_fd(i);
	if (this->_clients[x].channels.size() == 0)
		return false;
	in_channel = 0;
	for (unsigned int z = 0; z < this->_clients[x].channels.size(); z++)
	{
		for (unsigned int y = 0; y < this->_channels.size(); y++)
		{
			if (this->_clients[x].channels[z] == this->_channels[y].name)
			{
				if (this->_clients[x].operator_channels[z] == 1)
					in_channel = 1;
				else
					in_channel = 2;
				break;
			}
		}
		if (in_channel == 0)
		{
			message = ERR_CHANOPERISNEEDED(this->_clients[x].nickname, this->_last_cmd[1]);
			if(send(this->_clients[get_current_fd(i)].fd, message.c_str(), message.size(), 0) == -1)
				return false;
		}
	}
	if (in_channel == 1)
		message = RPL_CHANNELMODEIS(this->_clients[x].nickname, this->_last_cmd[1], "+nt " + this->_last_cmd[-1]);
	else if (in_channel == 2)
		message = RPL_CHANNELMODEIS(this->_clients[x].nickname, this->_last_cmd[1], "+n " + this->_last_cmd[-1]);
	if (send(this->_clients[x].fd, message.c_str(), message.size(), 0) == -1)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}
	return false;
}
**/
