SRCS_DIR = ./src/
SRCS = main.cpp
INCLUDES = -I ../nvimClient/  -I$(BOOST_ROOT)/include -I./msgpack-c/include
IMPL_HEADERS = ../nvimClient/impl/Client.hpp \
							 ../nvimClient/impl/MsgPacker.hpp \
							 ../nvimClient/impl/TcpConnector.hpp \
							 ../nvimClient/impl/CallDispatcher.hpp
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
