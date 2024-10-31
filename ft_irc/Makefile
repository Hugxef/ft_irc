BLU			= \033[0;34m
GRN			= \033[0;32m
RED			= \033[0;31m
RST			= \033[0m
END			= \e[0m
TOTEM 		= 🦁

FILES = main.cpp \
		irc_utils.cpp \
		IRC.cpp \
		IRC_Server.cpp \
		IRC_Channel.cpp \
		IRC_Users.cpp \
		IRC_Commands.cpp \
		IRC_Debug.cpp \
		IRC_Invite.cpp \
		IRC_Operators.cpp \
		IRC_Modes.cpp

SRCS = $(FILES:%=srcs/%)
NAME		= ircserv
OBJS_DIR	= objs/
# PROJECT_H	= srcs/ # Template : Header
OBJS		= $(SRCS:.cpp=.o)
OBJECTS_PREFIXED = $(addprefix $(OBJS_DIR), $(OBJS))
CC			= g++
CC_FLAGS	= -Wall -Werror -Wextra -std=c++98 -g3
LIBS		= # Add your libs here

$(OBJS_DIR)%.o : %.cpp $(PROJECT_H)
	@mkdir -p $(OBJS_DIR)
	@mkdir -p $(OBJS_DIR)srcs
	@$(CC) $(CC_FLAGS) -c $< -o $@
	@printf	"\033[2K\r${BLU}${TOTEM} [BUILD]${RST} '$<' $(END)"

$(NAME): $(OBJECTS_PREFIXED)
	@$(CC) -o $(NAME) $(OBJECTS_PREFIXED) $(CC_FLAGS) $(LIBS) # Program
	@printf "\033[2K\r\033[0;32m${TOTEM} [END]\033[0m $(NAME)$(END)\n"

all: $(NAME)

clean:
	@rm -rf $(OBJS_DIR)
	@printf "\033[2K\r${GRN}${TOTEM} [CLEAN]${RST} done$(END)\n"

fclean: clean
	@rm -f $(NAME)
	@rm -rf $(OBJS_DIR)
	@printf "\033[2K\r${GRN}${TOTEM} [FCLEAN]${RST} done$(END)\n"

re: fclean all

.PHONY:		all clean fclean re
