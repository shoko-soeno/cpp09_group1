#include "../ex01/RPN.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(RPNTest, SimpleAddition) {
    std::string expression = "3 4 +";
    EXPECT_NO_THROW(RPN::parseAndPushToken(expression));
}

TEST(RPNTest, InsufficientValues) {
    std::string expression = "5 +";
    try {
        RPN::parseAndPushToken(expression);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) { 
        EXPECT_STREQ("insufficient values in expression.", e.what());
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST(RPNTest, InvalidToken) {
    std::string expression = "(1 1 +)";
    try {
        RPN::parseAndPushToken(expression);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {         
        EXPECT_STREQ("invalid token: (1", e.what());
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

// overflow test
TEST(RPNTest, MultiplicationOverflow) {
    std::string expression = "8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 * * * * * * * * * * * * * * * * * * * * * *";
    try {
        RPN::parseAndPushToken(expression);
        FAIL() << "Expected std::overflow_error";
    } catch (const std::overflow_error& e) {         
        EXPECT_STREQ("multiplication overflow", e.what());
    } catch (...) {
        FAIL() << "Expected std::overflow_error";
    }
}

TEST(RPNTest, DivisionOverflow) {
    std::string expression = "0 2 - 4 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 * * * * * * * * * * * * * * * * * * * * * 0 1 - /";
    try {
        RPN::parseAndPushToken(expression);
        FAIL() << "Expected std::overflow_error";
    } catch (const std::overflow_error& e) {         
        EXPECT_STREQ("division overflow: LLONG_MIN / -1", e.what());
    } catch (...) {
        FAIL() << "Expected std::overflow_error";
    }
}
