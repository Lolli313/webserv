NAME = webserv

# si il faut changer le nom des directory faites le ici
DIR_DEP = dependances
DIR_INC = include
DIR_OBJ = objects
DIR_SRC = src
DIR_TPP = templates
# pas plus bas juste au-dessus

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98 -I$(DIR_INC) -I$(DIR_TPP)

SRCS = $(shell find $(DIR_SRC) -name "*.cpp" 2>/dev/null)
OBJS = $(patsubst $(DIR_SRC)/%.cpp, $(DIR_OBJ)/%.o, $(SRCS))
DEPS = $(patsubst $(DIR_SRC)/%.cpp, $(DIR_DEP)/%.d, $(SRCS))

IS_DIR_TPP = $(shell if [ -d "$(DIR_TPP)" ]; then echo "yes"; else echo "no"; fi)
ifeq ($(IS_DIR_TPP),yes)
TPPS = $(shell find $(DIR_TPP) -name "*.tpp" 2>/dev/null)
endif

all: $(NAME)

$(DIR_OBJ) $(DIR_DEP):
	@mkdir -p $(DIR_OBJ) $(DIR_DEP)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp | $(DIR_DEP) $(DIR_OBJ)
	@mkdir -p $(@D) $(DIR_DEP)/$(*D)
	@$(CXX) $(FLAGS) -MD -MP -MF $(DIR_DEP)/$*.d -c $< -o $@

ifdef TPPS
$(OBJS): $(TPPS)
endif

-include $(DEPS)

$(NAME): $(OBJS)
	@$(CXX) $(FLAGS) $^ -o $@

clean:
	@rm -rf $(DIR_OBJ) $(DIR_DEP)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

# SRC = main.cpp src/Tools.cpp src/ServerSocket.cpp src/Server.cpp src/Polling.cpp src/NetworkConfig.cpp src/Client.cpp

# NAME = webserv
# CXX = c++
# CXXFLAGS = -g -Wall -Wextra -Werror -std=c++98 -Iinclude
# OBJ = $(SRC:.cpp=.o)
# INCLUDE = -Iinclude

# all: $(NAME)

# $(NAME): $(OBJ)
# 	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# clean:
# 	rm -f $(OBJ)

# fclean: clean
# 	rm -f $(NAME)

# re: fclean all

# .PHONY: all clean fclean re
