NAME	= RPN
SRCS	= main.cpp RPN.cpp RPN.cpp

OBJS	= $(SRCS:.cpp=.o)

CXX	= c++
CXXFLAGS	= -Wall -Werror -Wextra
CSTD	= -std=c++98

.DEFAULT:	all
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CSTD) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CSTD) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

test:
	cmake -S .. -B ../build
	cmake --build ../build
	valgrind --leak-check=full ../build/ex01/ex01_test

.PHONY: all clean fclean re
