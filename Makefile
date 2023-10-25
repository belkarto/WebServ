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

# #================================= Files to compile ===================================
# SRC_FILES	= main Client Server ConfigParser ConfigManager Exception.cpp 
# LIB_FILES = signal split addr_resolver ft_stoll ft_stoi
# # #======================================================================================
#
# # #====================================== Colors ========================================
# NO_COLOR	= \033[0m
# GRAY		= \033[0;1;3;90m
# RED			= \033[0;1;3;91m
# GREEN		= \033[0;1;3;92m
# GREEN_L		= \033[0;1;3;36m
# YELLOW		= \033[0;1;3;33m
# BLUE		= \033[0;1;3;34m
# # #======================================================================================
#
# # #=================================files directories ===================================
# SRC_DIR		= src/
# LIB_DIR		= lib/
# OBJ_DIR		= objs/
# # #======================================================================================
#
#
# # #===================================== Standard =======================================
# NAME		= webserv
#
# AUTHOR		= BELKARTO
#
# CC			= c++
#
# OBJF		= .cache_exists
#
# FLAGS		=  -Wall -Wextra -Werror -std=c++98 -Iinclude
# #-g -fsanitize=address
# # #======================================================================================
#
#
# # # ===Better not to touch ===#
# SRC 		= 	$(addprefix $(SRC_DIR), $(addsuffix .cpp, $(SRC_FILES)))
# OBJ 		= 	$(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))
# # #===========================#
#
# all : header $(NAME)
#
# # # == Rule that compile source files into object files ==
# $(OBJ_DIR)%.o	: $(SRC_DIR)%.cpp | $(OBJF)
# 	@$(CC) $(FLAGS) -c $< -o $@
# 	@printf "$(GRAY)\r- Compiling ...⌛$(NO_COLOR)"
# 	@sleep 0.03
# 	@printf "$(GRAY)\r- Compiling ...⏳$(NO_COLOR)"
# 	@sleep 0.03
# # #=======================================================
#
# # #=== rule that compile the final program ===
# $(NAME) : $(OBJ)
# 	@$(CC) $(FLAGS) $(OBJ) -o $(NAME)
# 	@printf "$(GREEN)\n- Finished ✅🥳\n$(NO_COLOR)"
# # #===========================================
#
# # #=============================================================================#
#
# # #== rule that called if object folder doesn't exist ==
# $(OBJF):
# 	@mkdir -p $(OBJ_DIR)
# # #=====================================================
#
# ## # == rule deleting compiled files : the cache folder ==
# clean : header
# 	@rm -rf $(OBJ_DIR)
# 	@printf "$(BLUE)clean\t:\t\t\t$(GREEN)[✓]$(NO_COLOR)\n"
# # # =====================================================
#
# # # == Rule calling clean and deleting the final file ==
# fclean	: header clean
# 	@rm -f $(NAME)
# 	@printf "$(BLUE)fclean\t:\t\t\t$(GREEN)[✓]$(NO_COLOR)\n\n"
# # # ====================================================
#
# # # == HEADER DISPLAYS AUTHOR'S NAME AND COMILER AND FLAGS BEEN USED AND ===
# header:
# 	@echo
# 	@printf  "$(GREEN_L)Author\t: $(BLUE)$(AUTHOR)\n"
# 	@printf  "$(GREEN_L)CC    \t: $(YELLOW)$(CC)\n\033[m"
# 	@printf  "$(GREEN_L)Flags \t: $(YELLOW)$(FLAGS)\n\033[m"
# 	@echo
# # # ========================================================================
# re		:	fclean all
