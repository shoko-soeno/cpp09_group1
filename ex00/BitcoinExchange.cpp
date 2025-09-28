#include "BitcoinExchange.hpp"
#include "RateTable.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <cstdlib>    // strtod
#include <iomanip>

BitcoinExchange::BitcoinExchange() {}
BitcoinExchange::~BitcoinExchange() {}
BitcoinExchange::BitcoinExchange(const BitcoinExchange&) = default;
BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange&) = default;

static void printError(std::ostream& err, const std::string& msg) {
    err << "Error: " << msg << '\n';
}

bool BitcoinExchange::parseStrictDouble(const std::string& s, double& out) {
    const char* c = s.c_str();
    char* endp = 0;
    out = std::strtod(c, &endp);
    if (endp == c) return false; // 1文字も読めていない
    while (*endp) { // 末尾は空白のみ許容
        if (!std::isspace(*endp)) return false;
        ++endp;
    }
    return true;
}

// ヘッダ "date | value"（前後空白は許容）
bool BitcoinExchange::isHeaderLine(std::string s) {
    return trim(s) == "date | value";
}

// YYYY-MM-DD 形式・範囲チェック（うるう年対応）
static bool isLeap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

bool BitcoinExchange::isValidDate(const std::string& d) {
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int y, m, day;  // y -> year, m -> month, day -> day

    if (!BitcoinExchange::checkYYYYMMDD(d)) return false;
    BitcoinExchange::split_YYYY_MM_DD(d, y, m, day);
    if (m < 1 || m > 12) return false;
    int maxd = mdays[m-1];
    if (m==2 && isLeap(y)) maxd = 29;
    if (day < 1 || day > maxd) return false;
    return true;
}

bool BitcoinExchange::isValidValue(double v) {
    // Accept values between 0 and 1000 inclusive
    if (v < 0.0) return false;
    if (v > 1000.0) return false;
    return true;
}

void BitcoinExchange::run(std::istream& dbCsv, std::istream& input, std::ostream& out, std::ostream& err) {
    try {
        table_.load(dbCsv);
    } catch (const std::exception& e) {
        printError(err, e.what());
        return;
    }

    std::string line;
    bool header_checked = false;

    while (std::getline(input, line, '\n')) {
        try {
            if (line.empty()) continue;

            if (!header_checked) {
                header_checked = true;
                if (this->isHeaderLine(line)) continue; // ヘッダは読み飛ばす
            }

            // "date | value"
            std::string::size_type bar = line.find('|');
            if (bar == std::string::npos)
                throw std::runtime_error("bad input => " + line);

            std::string date = this->trim(line.substr(0, bar));
            std::string sval = this->trim(line.substr(bar + 1));

            if (!this->isValidDate(date))
                throw std::runtime_error("bad input => " + date);

            double val = 0.0;
            if (!this->parseStrictDouble(sval, val))
                throw std::runtime_error("bad input => " + sval);

            if (!this->isValidValue(val))
                throw std::runtime_error(val < 0 ? "not a positive number." : "too large a number.");

            double rate = 0.0;
            if (!table_.getRateForDate(date, rate))
                throw std::runtime_error("bad input => " + line); // 直近過去が存在しない（DBの最古日より前）

            double result = val * rate;
            out << date << " => " << val << " = " << result << '\n';
        } catch (const std::exception& e) {
            printError(err, e.what());
        }
    }
}

bool BitcoinExchange::checkYYYYMMDD(const std::string& d) {
    if (d.size() != DATE_TOTAL_LEN)
        return false;

    for (std::size_t j = 0; j < d.size(); j++) {
        if (j == DATE_YEAR_END || j == DATE_MONTH_END) {
            if (d[j] != '-')
                return false;
        } else {
            if (!std::isdigit(d[j]))
                return false;
        }
    }
    return true;
}

bool BitcoinExchange::split_YYYY_MM_DD(
      const std::string& d,
      int& y,
      int& m,
      int& day) {
    y = std::atoi(d.substr(0, DATE_YEAR_END).c_str());
    m = std::atoi(d.substr(DATE_YEAR_END + 1, DATE_MONTH_END - DATE_YEAR_END - 1).c_str());
    day = std::atoi(d.substr(DATE_MONTH_END + 1, DATE_TOTAL_LEN - DATE_MONTH_END - 1).c_str());
    return true;
}