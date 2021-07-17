NAME = server

C_SRC = src/Cluster.cpp \
		src/Connection.cpp \
		src/ParseConfig.cpp \
		src/RequestHandler.cpp \
		src/Response.cpp \
		src/Server.cpp \
		src/ServerConfig.cpp \
		src/get_next_line/get_next_line.cpp \
		src/get_next_line/get_next_line_utils.cpp \
		src/main.cpp

O_SRC = $(C_SRC:.cpp=.o)

FLAGS = -std=c++11 -Wall -Wextra -Werror -std=c++11

.PHONY: all clean fclean re debug

all: $(NAME)

$(NAME): $(O_SRC)
	clang++ -o $(NAME) $(FLAGS) $(O_SRC)

%.o: %.cpp
	clang++ $(FLAGS) --include-directory=src --include-directory=includes  -c $< -o $@

clean:
	/bin/rm -f $(O_SRC)

fclean: clean
	/bin/rm -f $(NAME)

re: fclean all
