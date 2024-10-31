/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Invite.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 11:26:12 by thomathi          #+#    #+#             */
/*   Updated: 2023/07/18 17:39:36 by hugrene          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"

bool IRC::invite_user(int i)
{
	int sender_pos;
	std::string sender_nickname;
	std::string sender_username;
	std::string receiver_nickname;
	std::string channel;
	std::string message;

	sender_pos = get_current_fd(i);
	sender_nickname = this->_clients[sender_pos].nickname;
	sender_username = this->_clients[sender_pos].username;
	receiver_nickname = this->_last_cmd[1];
	if (this->_last_cmd[2][0] != '#')
	{
		message = ERR_NOSUCHCHANNEL(sender_nickname, channel);
		if (send(this->_clients[sender_pos].fd, message.c_str(), message.size(), 0) == -1)
		return false;
	}
	channel = this->_last_cmd[2];
	if (channel.empty())
	{
		message = ERR_NEEDMOREPARAMS(sender_nickname, receiver_nickname);
		if (send(this->_clients[sender_pos].fd, message.c_str(), message.size(), 0) == -1)
		return false;
	}
	message = RPL_INVITING(user_id(sender_nickname, sender_username), sender_nickname, receiver_nickname, channel);
	if (send(this->_clients[sender_pos].fd, message.c_str(), message.size(), 0) == -1)
	{
		std::cout << "Error: send() failed" << std::endl;
		return true;
	}	
	for (unsigned int z = 0; z < this->_clients.size(); z++)
	{
		if (this->_clients[z].nickname == receiver_nickname)
		{
			message = RPL_INVITE(user_id(sender_nickname, sender_username), receiver_nickname, channel);
			if (send(this->_clients[z].fd, message.c_str(), message.size(), 0) == -1)
			{
				std::cout << "Error: send() failed" << std::endl;
				return true;
			}
		}
		else 
		{
			message = ERR_NOTONCHANNEL(sender_nickname, channel);
			if (send(this->_clients[sender_pos].fd, message.c_str(), message.size(), 0) == -1)
			return false;
		}
	}
	message = ERR_USERONCHANNEL(sender_nickname, receiver_nickname, channel);
	if (send(this->_clients[sender_pos].fd, message.c_str(), message.size(), 0) == -1)
		return false;
	return false;
}
