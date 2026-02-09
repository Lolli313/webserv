SRC = main.cpp 

NAME = webserv
CXX = c++
CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98 -Iinclude
OBJ = $(SRC:.cpp=.o)
INCLUDE = -Iinclude

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
