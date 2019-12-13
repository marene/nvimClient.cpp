BOOST_ROOT = /home/matthieu/lib//boost_1_70_0
SRCS_DIR = ./src/
SRCS = main.cpp \
	   Client.cpp \
	   TcpConnector.cpp
INCLUDES = -I ./include/ -I./neovim/src -I$(BOOST_ROOT) -I./msgpack-c/include
LIBRARIES = -L/home/matthieu/lib/boost_1_70_0/stage/lib -lboost_system -lpthread -lcurses
OBJ_DIR = ./obj/
OBJS = $(SRCS:.cpp=.o)
NAME = rpcVim


all: $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	mkdir -p $@

$(NAME): $(addprefix $(OBJ_DIR), $(OBJS))
	clang++ -std=c++14 -g $^ $(INCLUDES) $(LIBRARIES) -o $@

$(OBJ_DIR)%.o: $(SRCS_DIR)%.cpp
	clang++ -std=c++14 $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(NAME)

fclean: clean
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
