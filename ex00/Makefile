NAME	= btc
SRCS	= main.cpp BitcoinExchange.cpp RateTable.cpp

OBJS	= $(SRCS:.cpp=.o)

CXX	= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98

.DEFAULT:	all
all: $(NAME)

# link object files to create the executable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

test:
	cmake -S .. -B ../build
	cmake --build ../build
	cd ../build && ctest
# valgrind --leak-check=full ../build/tests/ex00/ex00_test

.PHONY: all clean fclean re test