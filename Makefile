NAME		=	webserv
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98
RM			=	rm -rf

SRC_DIR		=	src
SRC_FILES	=	main.cpp Client.cpp Server.cpp ConfigParser.cpp ConfigManager.cpp \
				Exception.cpp
SRC_OBJS	=	$(addprefix $(SRC_DIR)/, $(SRC_FILES:.cpp=.o))

LIB_DIR		=	lib
LIB_FILES	=	signal.cpp	split.cpp	addr_resolver.cpp	ft_stoll.cpp ft_stoi.cpp
LIB_OBJS	=	$(addprefix $(LIB_DIR)/, $(LIB_FILES:.cpp=.o))


OBJS		= $(SRC_OBJS) $(LIB_OBJS) 

all: $(NAME)

$(NAME): CXXFLAGS += -Iinclude
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)


$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all
