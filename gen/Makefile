srcs_dir = ./src/
srcs = main.cpp \
			 generator.cpp \
			 nvimApiMetadata.cpp
includes = -I ./include/ -I../msgpack-c/include
obj_dir = ./obj/
objs = $(srcs:.cpp=.o)
name = gen


all: $(obj_dir) $(impl_headers) $(name)

$(obj_dir):
	mkdir -p $@

$(name): $(addprefix $(obj_dir), $(objs))
	clang++ -std=c++17 -g $^ $(includes) -o $@

$(obj_dir)%.o: $(srcs_dir)%.cpp
	clang++ -std=c++17 $(includes) -c -o $@ $<

clean:
	rm -f $(name)

fclean: clean
	rm -rf $(obj_dir)

re: fclean all

.phony: all clean fclean re

foobar:
	clang++ --std=c++17 -I../msgpack-c/include main.cpp nvimApiMetadata.cpp generator.cpp -g -o $(NAME)
