#include "RPN.hpp"

RPN::RPN() {}
RPN::~RPN() {}
RPN::RPN(const RPN& src) {
    *this = src;
}
RPN& RPN::operator=(const RPN& src){
    if (this != &src) {
        this->stack_ = src.stack_;
    }
    return *this;
}
