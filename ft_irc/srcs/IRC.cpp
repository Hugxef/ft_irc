/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 20:15:03 by thomathi          #+#    #+#             */
/*   Updated: 2023/08/01 19:05:02 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "ft_irc.hpp"
#include <errno.h>
#include <cstring>
#include <sys/poll.h>

IRC::IRC()
{}

IRC::~IRC()
{}

IRC::IRC(const IRC &copy)
{
	this->_server = copy._server;
	this->_socket = copy._socket;
}

IRC &IRC::operator=(const IRC &copy)
{
	if (this != &copy)
	{
		this->_server = copy._server;
		this->_socket = copy._socket;
	}
	return *this;
}

void IRC::create_socket()
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket == -1)
		guru_meditation(3);
}

void IRC::create_server(int port)
{
	this->_server.sin_family = AF_INET;
	this->_server.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &this->_server.sin_addr);
}

void IRC::set_password(std::string pass)
{
	this->_password = pass;
}

void IRC::bind_server()
{
	int result;
	result = bind(this->_socket, (sockaddr *) &this->_server, sizeof(this->_server));
	if (result == -1)
		guru_meditation(4);
}

void IRC::listen_socket()
{
	int result;

	result = listen(this->_socket, SOMAXCONN);
	if (result == -1)
		guru_meditation(5);
}

void IRC::server_loop()
{
	int timeout, nfds, result, current_size, new_sd, loop;
	bool close_conn, compress_array, kill_everything = false;
	char buffer[4096];
	std::stringstream num;
	
	memset(this->_pollfd, 0, sizeof(this->_pollfd));
	this->_pollfd[0].fd = this->_socket;
	this->_pollfd[0].events = POLLIN;
	this->_amount_clients = 0;
	this->_amount_channels = 0;
	timeout = (667 * 'E' * 'K'* 'I' + 'P');
	nfds = 1;
	current_size = 0;
	loop = 1145;
	while (loop == 1145)
	{
		std::cout << "Waiting on poll()..." << std::endl;
		result = poll(this->_pollfd, nfds, timeout);
		if (result < 0)
		{
			std::cout << "Error: poll() failed" << std::endl;
			break;
		}
		if (result == 0)
		{
			std::cout << "Error: poll() timed out." << std::endl;
			break;
		}
		current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			if (this->_pollfd[i].revents == 0)
				continue;
			if (this->_pollfd[i].revents != POLLIN && this->_pollfd[i].revents != 32 && this->_pollfd[i].revents != 25)
			{
				std::cout << "Error: revents = " << this->_pollfd[i].revents << std::endl;
				exit(6);
			}
			if (this->_pollfd[i].fd == this->_socket)
			{
				std::cout << "Listening socket is readable" << std::endl;
				new_sd = accept(this->_socket, NULL, NULL);
				if (new_sd < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						std::cout << "Error: accept() failed" << std::endl;
						exit(7);
					}
				}
				std::cout << "New incoming connection -> " << new_sd << std::endl;
				this->_clients.push_back(Client());
				this->_clients[this->_amount_clients].fd = new_sd;
				num.str(std::string());
				num << this->_amount_clients;
				this->_clients[this->_amount_clients].nickname = "Unamed" + num.str();
				this->_clients[this->_amount_clients].username = "Unamed" + num.str();
				this->_clients[this->_amount_clients].welcome_done = 0;
				this->_clients[this->_amount_clients].pass_done = 1;
				this->_pollfd[nfds].fd = new_sd;
				this->_pollfd[nfds].events = POLLIN;
				nfds++;
				this->_amount_clients++;
			}
			else
			{
				memset(buffer, 0, sizeof(buffer));
				std::cout << "Descriptor " << this->_pollfd[i].fd << " is readable" << std::endl;
				close_conn = false;
				compress_array = false;
				result = recv(this->_pollfd[i].fd, buffer, sizeof(buffer), 0);
				if (result < 0)
				{
					if (errno != EWOULDBLOCK)
					{
						std::cout << "Error: recv() failed" << std::endl;
						close_conn = true;
					}
				}
				if (result == 0)
				{
					std::cout << "Connection closed" << std::endl;
					close_conn = true;
				}
				parse_cmd(buffer);
				switch(cmd_select(this->_last_cmd[0]))
				{
					case 1145:
						debug_infos();
						break;
					case 1464:
						kill_everything = true;
						break;
					case 1:
						close_conn = join_channel(i);
						break;
					case 2:
						close_conn = priv_message(buffer, i);
						break;
					case 3:
						change_nickname(buffer, i);
						break;
					case 4:
						close_conn = quit_irc(buffer, i);
						break;
					case 5:
						close_conn = who_in_channel(i);
						break;
					case 6:
						close_conn = part_channel(i);
						break;
					case 7:
						close_conn = ping_pong(i);
						break;
					case 8:
						close_conn = cap_ls_bs(i);
						break;
					case 9:
						close_conn = invite_user(i);
						break;
					case 10:
						close_conn = kick(i);
						break;
					case 11:
						close_conn = topic(buffer, i);
						break;
					case 12:
						close_conn = channel_mode_args(i);
						break;
					case 13:
						close_conn = user_mode(i);
						break;
					case 14:
						close_conn = notice(buffer, i);
						break;
					case 15:
						change_username(i);
						if (this->_clients[get_current_fd(i)].pass_done == 1)
							welcome_message(i);
						else
							close_conn = true;
						break;
					case 16:
						is_pass_correct(i);
						break;
					default:
						print_debug(buffer);
						break;
				}
				if (kill_everything)
				{
					for (unsigned x = 0; x < this->_clients.size(); x++)
					{
						close(this->_clients[x].fd);
						this->_pollfd[x].fd = -1;
						compress_array = true;
						this->_amount_clients--;
					}
					loop = 1464;
				}
				if (close_conn)
				{
					for (unsigned x = 0; x < this->_clients.size(); x++)
					{
						if (this->_clients[x].fd == this->_pollfd[i].fd)
							this->_clients.erase(this->_clients.begin() + x);
					}
					close(this->_pollfd[i].fd);
					this->_pollfd[i].fd = -1;
					compress_array = true;
					this->_amount_clients--;
					continue;
				}
			}
		}
		if (compress_array)
		{
			compress_array = false;
			for (int i = 0; i < nfds; i++)
			{
				if (this->_pollfd[i].fd == -1)
				{
					for (int j = i; j < nfds; j++)
					{
						this->_pollfd[j].fd = this->_pollfd[j + 1].fd;
					}
					i--;
					nfds--;
				}
			}
		}
	}
	for (int i = 0; i < nfds; i++)
	{
		if (this->_pollfd[i].fd >= 0)
			close(this->_pollfd[i].fd);
	}
}

int IRC::get_socket()
{
	return this->_socket;
}
