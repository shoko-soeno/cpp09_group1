#include "BitcoinExchange.hpp"
#include "Utils.hpp"

BitcoinExchange::BitcoinExchange(std::istream& dbCsv) {
    table_.load(dbCsv);
}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& src) {
    this->table_ = src.table_;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& src){
    if (this != &src) {
        this->table_ = src.table_;
    }
    return *this;
}

bool BitcoinExchange::parseDouble(const std::string& sval, double& out) {
    const char* c = sval.c_str();
    char* endp = 0;
    out = std::strtod(c, &endp);
    if (endp == c) return false; // 1文字も読めていない
    while (*endp) { // 末尾は空白のみ許容
        if (!std::isspace(*endp)) return false;
        ++endp;
    }
    return true;
}

// header "date | value"
bool BitcoinExchange::isHeaderLine(std::string s) {
    return trim(s) == "date | value";
}

static bool isLeap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

// YYYY-MM-DD format check
bool BitcoinExchange::isValidDate(const std::string& date) {
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int y, m, day;  // y -> year, m -> month, day -> day

    if (!BitcoinExchange::checkYMD(date)) return false;
    BitcoinExchange::splitYMD(date, y, m, day);
    if (m < 1 || m > 12) return false;
    int maxd = mdays[m-1];
    if (m==2 && isLeap(y)) maxd = 29;
    if (day < 1 || day > maxd) return false;
    return true;
}

bool BitcoinExchange::isValidValue(double value) {
    // Accept values between 0 and 1000 inclusive
    if (value < 0.0) return false;
    if (value > 1000.0) return false;
    return true;
}

bool BitcoinExchange::checkYMD(const std::string& date) {
    if (date.size() != DATE_TOTAL_LEN) return false;

    for (std::size_t i = 0; i < date.size(); i++) {
        if (i == DATE_YEAR_END || i == DATE_MONTH_END) {
            if (date[i] != '-') return false;
        } else {
            if (!std::isdigit(date[i])) return false;
        }
    }
    return true;
}

bool BitcoinExchange::splitYMD(
      const std::string& date,
      int& y,
      int& m,
      int& day) {
    y = std::atoi(date.substr(0, DATE_YEAR_END).c_str());
    m = std::atoi(date.substr(DATE_YEAR_END + 1, DATE_MONTH_END - DATE_YEAR_END - 1).c_str());
    day = std::atoi(date.substr(DATE_MONTH_END + 1, DATE_TOTAL_LEN - DATE_MONTH_END - 1).c_str());
    return true;
}

void BitcoinExchange::run(std::istream& input, std::ostream& out, std::ostream& err)
{
    std::string line;
    bool header_checked = false;

    while (std::getline(input, line, '\n')) {
        try {
            if (line.empty()) continue;
            if (!header_checked) {
                header_checked = true;
                if (this->isHeaderLine(line)) continue;
            }
            // "date | value"
            std::string::size_type bar = line.find('|');
            if (bar == std::string::npos)
                throw std::runtime_error("bad input => " + line);
            std::string date = trim(line.substr(0, bar));
            std::string sval = trim(line.substr(bar + 1));
            if (!this->isValidDate(date))
                throw std::runtime_error("bad input => " + date);
            double val = 0.0;
            if (!this->parseDouble(sval, val))
                throw std::runtime_error("bad input => " + sval);
            if (!this->isValidValue(val))
                throw std::runtime_error(val < 0 ? "not a positive number." : "too large a number.");
            double rate = 0.0;
            if (!table_.getRateForDate(date, rate))
                throw std::runtime_error("bad input => " + line); // no rate found
            double result = val * rate;
            out << date << " => " << val << " = " << result << '\n';
        } catch (const std::exception& e) {
            printError(err, e.what());
        }
    }
}