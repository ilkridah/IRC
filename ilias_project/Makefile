NAME			= IRCserv

SRC				= 	main.cpp \
					socket/Server.cpp \
					socket/Poll.cpp \
					socket/Client.cpp \
					Parser.cpp\
					IRC.cpp  \
					Channel.cpp \
					IRChelper.cpp \
					authentification.cpp \
					Parser_authentification.cpp \
					Parser_channel.cpp \
					Parser_tools.cpp \
					bot.cpp \
					IRCReplies.cpp \
					 	
HEADERS			= ./socket/Socket.hpp IRCExecptions.hpp IRCReplies.hpp Channel.hpp IRC.hpp Parser.hpp\
					 ./socket/Client.hpp ./socket/Poll.hpp ./socket/Server.hpp ./MainHeader.hpp\

CXX				= c++
VERSION			= -std=c++98

CXXFLAGS		= -Wall -Wextra -Werror -Wshadow $(VERSION) -Wuninitialized -fsanitize=address

OBJ				= $(SRC:.cpp=.o)	

RM				= rm -f

all:	$(NAME)

run: all
	echo 5000 pass && \
	./$(NAME) 5000 pass

$(NAME):		$(OBJ) $(HEADERS)
				$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) -lcurl

clean:
				$(RM) $(OBJ)  

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

.PHONY : all clean fclean re