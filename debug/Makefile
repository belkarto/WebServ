name = server

CC = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++11

SRC = Server.cpp main.cpp

OBJ = $(SRC:.cpp=.o)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

all : $(name)

$(name) : $(OBJ)
	$(CC) $(CPPFLAGS) $(SRC) -o $(name)

clean :
	@rm -f $(OBJ)

fclean : clean
	@rm -rf $(name)

re : fclean all
