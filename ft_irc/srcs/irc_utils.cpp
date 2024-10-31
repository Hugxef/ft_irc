/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 14:00:52 by thomathi          #+#    #+#             */
/*   Updated: 2023/06/01 09:57:29 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"
#include <iostream>

bool is_digit(const std::string &arg)
{
	if (arg.find_first_not_of("0123456789") == std::string::npos)
		return true;
	return false;
}

void guru_meditation(int err)
{
	switch (err)
	{
		case 3:
			std::cout << "Error: Socket can't be created" << std::endl;
			break;
		case 4:
			std::cout << "Error: Binding failed" << std::endl;
			break;
		case 5:
			std::cout << "Error: Can't listen on the socket" << std::endl;
			break;
		default:
			break;
	}
	exit(err);
}
