SRCS_DIR = ./src/
SRCS = main.cpp
INCLUDES = -I ./include/ -I./neovim/src -I$(BOOST_ROOT)/include -I./msgpack-c/include
IMPL_HEADERS = ./include/impl/Client.hpp \
							 ./include/impl/MsgPacker.hpp \
							 ./include/impl/TcpConnector.hpp \
							 ./include/impl/CallDispatcher.hpp
LIBRARIES = -Wl,-rpath $(BOOST_ROOT)/lib -L$(BOOST_ROOT)/lib -lboost_system -lpthread -lcurses
OBJ_DIR = ./obj/
OBJS = $(SRCS:.cpp=.o)
NAME = rpcVim


all: $(OBJ_DIR) $(IMPL_HEADERS) $(NAME)

$(OBJ_DIR):
	mkdir -p $@

$(NAME): $(addprefix $(OBJ_DIR), $(OBJS))
	clang++ -std=c++17 -g $^ $(INCLUDES) $(LIBRARIES) -o $@

$(OBJ_DIR)%.o: $(SRCS_DIR)%.cpp
	clang++ -std=c++17 $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(NAME)

fclean: clean
	rm -rf $(OBJ_DIR)

re: fclean all

.PHONY: all clean fclean re
