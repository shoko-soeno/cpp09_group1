NAME	= PmergeMe
SRCS	= main.cpp

OBJS	= $(SRCS:.cpp=.o)

CXX	= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -DDEBUG

.DEFAULT:	all
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

test:
	cmake -S .. -B ../build
	cmake --build ../build
	valgrind --leak-check=full ../build/ex02/ex02_test

.PHONY: all clean fclean re
