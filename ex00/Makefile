NAME	= btc
SRCS	= main.cpp BitcoinExchange.cpp RateTable.cpp

OBJS	= $(SRCS:.cpp=.o)

CXX	= c++
CXXFLAGS	= -Wall -Werror -Wextra
CSTD	= -std=c++98

.DEFAULT:	all
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CSTD) -o $(NAME) $(OBJS)

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
	valgrind --leak-check=full ../build/ex00/ex00_test

.PHONY: all clean fclean re
