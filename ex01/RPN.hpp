#ifndef RPN_HPP
#define RPN_HPP
#include <string>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <stack>
#include <iostream>
#include <limits>
#include <list>

class RPN {
private:
    RPN();
    RPN(const RPN&);
    RPN& operator=(const RPN& src);
    ~RPN();

    static bool isOperator(const char c);
    static long long processOperator(const char op, long long a, long long b);

public:
    static void parseAndPushToken(const std::string& expresstion);
};

#endif // RPN_HPP