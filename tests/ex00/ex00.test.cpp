#include "../ex00/BitcoinExchange.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(BitcoinExchangeTest, ValidDate) {
    EXPECT_TRUE(BitcoinExchange::isValidDate("2023-01-01"));
    EXPECT_TRUE(BitcoinExchange::isValidDate("2000-02-29")); // Leap year
    EXPECT_FALSE(BitcoinExchange::isValidDate("2023-13-01")); // Invalid month
    EXPECT_FALSE(BitcoinExchange::isValidDate("2023-02-30")); // Invalid day
    EXPECT_FALSE(BitcoinExchange::isValidDate("abcd-ef-gh")); // Non-numeric
}

TEST(BitcoinExchangeTest, ValidValue) {
    EXPECT_TRUE(BitcoinExchange::isValidValue(0.0));
    EXPECT_TRUE(BitcoinExchange::isValidValue(1000.0));
    EXPECT_TRUE(BitcoinExchange::isValidValue(999.99));
    EXPECT_FALSE(BitcoinExchange::isValidValue(-1.0));
    EXPECT_FALSE(BitcoinExchange::isValidValue(1001.0));
    EXPECT_FALSE(BitcoinExchange::isValidValue(2147483648.0)); // Out of int range
}

TEST(BitcoinExchangeTest, CalculationOutput) {
    static const std::string answer_out =
        "2011-01-03 => 3 = 0.9\n"
        "2011-01-03 => 2 = 0.6\n"
        "2011-01-03 => 1 = 0.3\n"
        "2011-01-03 => 1.2 = 0.36\n"
        "2011-01-09 => 1 = 0.32\n"
        "2012-01-11 => 1 = 7.1\n";
    static const std::string answer_err =
        "Error: not a positive number.\n"
        "Error: bad input => 2001-42-42\n"
        "Error: too large a number.\n";

    std::istringstream db("2011-01-03,0.3\n2011-01-09,0.32\n2012-01-11,7.1\n");
    std::istringstream input(
        "date | value\n"
        "2011-01-03 | 3\n"
        "2011-01-03 | 2\n"
        "2011-01-03 | 1\n"
        "2011-01-03 | 1.2\n"
        "2011-01-09 | 1\n"
        "2012-01-11 | -1\n"
        "2001-42-42\n"
        "2012-01-11 | 1\n"
        "2012-01-11 | 2147483648\n"
    );
    std::ostringstream out, err;
    BitcoinExchange app;
    app.run(db, input, out, err);

    EXPECT_EQ(out.str(), answer_out);
    EXPECT_EQ(err.str(), answer_err);
}
