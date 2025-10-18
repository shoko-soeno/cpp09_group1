#include "RPN.hpp"

RPN::RPN() {}
RPN::~RPN() {}
RPN::RPN(const RPN& dummy) {
    (void)dummy;
}
RPN& RPN::operator=(const RPN& dummy){
    (void)dummy;
    return *this;
}

bool RPN::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool willMulOverflow(long long a, long long b) {
    if (a == 0 || b == 0) return false;
    __int128 t = static_cast<__int128>(a) * static_cast<__int128>(b);
    return ( t > std::numeric_limits<long long>::max() ||
           t < std::numeric_limits<long long>::min());
}

long long RPN::processOperator(char op, long long a, long long b) {
    switch (op) {
        case '+': return b + a;
        case '-': return b - a;
        case '*':
            if (willMulOverflow(b, a))
                throw std::overflow_error("multiplication overflow");
            return b * a;
        case '/':
            if (a == 0)
                throw std::runtime_error("division by zero.");
            if (b == std::numeric_limits<long long>::min() && a == -1)
                throw std::overflow_error("division overflow: LLONG_MIN / -1");
            return b / a;
        default:
            throw std::runtime_error(std::string("unknown operator: ") + op);
            return 0;
    }
}

void RPN::parseAndPushToken(const std::string& expresstion) {
    std::stringstream ss(expresstion);
    std::string token;
    std::stack<long long, std::list<long long> > st;

    while (ss >> token) {
        if (std::isdigit(token[0])) {
            if (token.size() > 1) throw std::runtime_error("invalid token: " + token);
            st.push(token[0] - '0');
        } else if (isOperator(token[0]) && token.size() == 1) {
            if (st.size() < 2)
                throw std::runtime_error("insufficient values in expression.");
            long long a = st.top();
            st.pop();
            long long b = st.top();
            st.pop();
            st.push(processOperator(token[0], a, b));
        } else {
            throw std::runtime_error("invalid token: " + token);
        }
    }
    if (st.size() != 1)
        throw std::runtime_error("the input has too many values.");
    std::cout << st.top() << std::endl;
}