/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 13:58:22 by thomathi          #+#    #+#             */
/*   Updated: 2023/07/28 01:16:30 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ft_irc.hpp"
#include "IRC.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage : " << argv[0] << " <server port> <server password>" << std::endl;
		return 1;
	}
	else if (!is_digit(argv[1]))
	{
		std::cout << "Error: Server port argument should only be numbers" << std::endl;
		return 2;
	}

	// Create a socket and a server
	IRC *irc = new IRC;
	irc->create_socket();
	irc->create_server(atoi(argv[1]));
	irc->set_password(argv[2]);

	// Bind the socket to ip / port
	irc->bind_server();

	// Mark the socket for listening
	irc->listen_socket();

	// Accept connection
	irc->server_loop();

	delete irc;
	return 0;
}
