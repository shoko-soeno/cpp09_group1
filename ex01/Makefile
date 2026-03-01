NAME	= RPN
SRCS	= main.cpp RPN.cpp

OBJS	= $(SRCS:.cpp=.o)

CXX	= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98

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
	cd ../build && ctest
#	valgrind --leak-check=full ../build/ex01/ex01_test

.PHONY: all clean fclean re test
