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

TEST(BitcoinExchangeTest, InputEdgeCases) {
    std::istringstream db("2012-02-29,1.0\n2011-01-03,0.3\n2011-01-09,0.32\n2012-01-11,7.1\n");
    std::istringstream input(
        "date | value\n"
        "2011-01-03 | abc\n"           // non-numeric value
        "2011-01-03 |\n"               // missing value
        "| 1\n"                        // missing date
        "2011-01-03|1\n"               // no spaces around bar
        "2011-01-03 | 1 | extra\n"     // extra bar
        "2011-01-03\n"                 // missing bar and value
        "2011-02-29 | 1\n"             // invalid date (not leap year)
        "2012-02-29 | 1\n"             // valid leap year date
        "2011-13-01 | 1\n"             // invalid month
        "2011-00-01 | 1\n"             // invalid month (zero)
        "2011-01-32 | 1\n"             // invalid day
        "2011-01-00 | 1\n"             // invalid day (zero)
        "2011-01-03 | 0\n"             // boundary: zero value
        "2011-01-03 | 1000\n"          // boundary: max valid value
        "2011-01-03 | 1000.01\n"       // just above max
    );
    std::ostringstream out, err;
    BitcoinExchange app;
    app.run(db, input, out, err);

    // You can check for specific error messages or valid outputs
    std::string errors = err.str();
    EXPECT_NE(errors.find("Error: bad input => abc"), std::string::npos);
    EXPECT_NE(errors.find("Error: bad input => "), std::string::npos); // for missing value/date
    EXPECT_NE(errors.find("Error: bad input => 2011-13-01"), std::string::npos);
    EXPECT_NE(errors.find("Error: bad input => 2011-00-01"), std::string::npos);
    EXPECT_NE(errors.find("Error: bad input => 2011-01-32"), std::string::npos);
    EXPECT_NE(errors.find("Error: bad input => 2011-01-00"), std::string::npos);
    EXPECT_NE(errors.find("Error: too large a number."), std::string::npos);

    std::string output = out.str();
    EXPECT_NE(output.find("2012-02-29 => 1 = 1"), std::string::npos); // valid leap year
    EXPECT_NE(output.find("2011-01-03 => 0 = 0"), std::string::npos); // boundary zero
    EXPECT_NE(output.find("2011-01-03 => 1000 = 300"), std::string::npos); // boundary max
}
