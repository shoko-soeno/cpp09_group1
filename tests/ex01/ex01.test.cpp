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
    std::string expression = "2 a +";
    try {
        RPN::parseAndPushToken(expression);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {         
        EXPECT_STREQ("invalid token: a", e.what());
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}
