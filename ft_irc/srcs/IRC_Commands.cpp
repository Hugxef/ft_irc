/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC_Commands.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 14:33:49 by thomathi          #+#    #+#             */
/*   Updated: 2023/08/01 19:00:46 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"

int IRC::cmd_select(std::string cmd)
{
	if (cmd.compare("dieu") == 0)
		return 1145;
	if (cmd.compare("satan") == 0)
		return 1464;
	if (cmd.compare("JOIN") == 0)
		return 1;
	if (cmd.compare("PRIVMSG") == 0)
		return 2;
	if (cmd.compare("NICK") == 0)
		return 3;
	if (cmd.compare("QUIT") == 0)
		return 4;
	if (cmd.compare("WHO") == 0)
		return 5;
	if (cmd.compare("PART") == 0)
		return 6;
	if (cmd.compare("PING") == 0)
		return 7;
	if (cmd.compare("CAP") == 0)
		return 8;
	if (cmd.compare("INVITE") == 0)
		return 9;
	if (cmd.compare("KICK") == 0)
		return 10;
	if (cmd.compare("TOPIC") == 0)
		return 11;
	if (cmd.compare("MODE") == 0)
		return 12;
	if (cmd.compare("NOTICE") == 0)
		return 14;
	if (cmd.compare("USER") == 0)
		return 15;
	if (cmd.compare("PASS") == 0)
		return 16;
	else
		return 0;
}

void IRC::parse_cmd(char *buffer)
{
	std::string parsed_string;
	int			buffer_len = 0;

	this->_last_cmd.clear();
	while (buffer[buffer_len] != '\r' && buffer[buffer_len] != '\n' && buffer[buffer_len])
		buffer_len++;
	parsed_string = "";
	for (int i = 0; i < buffer_len; i++)
	{
		if (buffer[i] == ' ')
		{
			this->_last_cmd.push_back(parsed_string);
			parsed_string = "";
			continue;
		}
		parsed_string.push_back(buffer[i]);
	}
	this->_last_cmd.push_back(parsed_string);
	std::cout << "Cmd parse : [";
	for (unsigned i = 0; i < this->_last_cmd.size(); i++)
	{
		std::cout << "\"" << this->_last_cmd[i] << "\", ";
	}
	std::cout << "]" << std::endl;
	return ;
}

void IRC::unknown_cmd(int i)
{
	std::string	message;
	int			x;
	int			result;

	x = get_current_fd(i);
	message = ERR_UNKNOWNCOMMAND(user_id(this->_clients[x].nickname, this->_clients[x].username), this->_last_cmd[0]);
	result = send(this->_clients[x].fd, message.c_str(), message.size(), 0);
	if (result == -1)
	{
		std::cout << "Error : send() failed" << std::endl;
		return ;
	}
	return ;
}
