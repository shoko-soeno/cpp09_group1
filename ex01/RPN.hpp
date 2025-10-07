#ifndef RPN_HPP
#define RPN_HPP
#include <string>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <stack>
#include <iostream>

#define OPERATORS "+-/*"

class RPN {
public:
    RPN();
    RPN(const RPN&);
    RPN& operator=(const RPN& src);
    ~RPN();
    
private:
    std::stack<int> stack_;
    bool isOperator(char c);
    int applyOperator(int left, int right, char op);
}