/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRC.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thomathi <thomathi@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/31 20:05:15 by thomathi          #+#    #+#             */
/*   Updated: 2023/08/01 10:31:46 by thomathi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

#include "ft_irc.hpp"
#include <sys/poll.h>
#include <vector>
#include <sstream>

struct Client {
	std::string					nickname;
	std::string					username;
	std::vector<std::string>	channels;
	std::vector<std::string>	mode_channel;
	int							fd;
	int							welcome_done;
	int							pass_done;
};

struct Channel {
	std::string	name;
	int			users_count;
	std::string	topic;
	std::string	rpl_topic;
};

class IRC {
	public:
		IRC();
		IRC(const IRC &copy);
		IRC &operator=(const IRC &copy);
		~IRC();
		int			get_socket();
		void		create_socket();
		void		create_server(int port);
		void		set_password(std::string pass);
		void		bind_server();
		void		listen_socket();
		void		server_loop();

		// Debug
		void		debug_infos();
		void		print_debug(char *buffer);

		// Command
		int			cmd_select(std::string cmd);
		void		parse_cmd(char *buffer);
		void		unknown_cmd(int i);

		// Channel
		bool		join_channel(int i);
		bool		part_channel(int i);
		bool		who_in_channel(int i);
		std::string	get_users_in_channel(std::string channel);
		int			is_new_channel(std::string channel);
		bool		check_is_in_channel(int i, std::string channel);
		bool		notice(char *buffer, int i);
		bool		priv_message(char *buffer, int i);
		bool		avoid_dup_channel(int i, std::string channel);
		int			get_channel_id(std::string channel);
		bool		is_operator(int i, std::string channel);

		// Server
		bool		welcome_message(int i);
		bool		ping_pong(int i);
		bool		quit_irc(char *buffer, int i);
		bool		cap_ls_bs(int i);
		bool		is_pass_correct(int i);

		// User
		void		change_nickname(char *buffer, int i);
		void		change_username(int i);
		int			get_current_fd(int i);

		// Invite
		bool		invite_user(int i);

		// Operators
		bool		kick(int i);
		bool		topic(char *buffer, int i);

		// Modes
		bool		channel_mode(int i);
		bool		user_mode(int i);
		bool		channel_mode_args(int i);
		void		set_new_modes(std::string elem, int user_id, int chan_id);
			
	private:
		sockaddr_in					_server;
		int							_socket;
		std::string					_password;
		struct pollfd				_pollfd[4096];
		std::vector<Client> 		_clients;
		int							_amount_clients;
		std::vector<std::string>	_last_cmd;
		std::vector<Channel>		_channels;
		int							_amount_channels;
};

#endif
