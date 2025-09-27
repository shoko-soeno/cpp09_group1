#include "BitcoinExchange.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <cstdlib>    // strtod
#include <iomanip>

/////////////////////////// RateTableの実装 ///////////////////////////////// 
static void trim(std::string& s) {
    std::string::size_type b = s.find_first_not_of(" \t\r\n");
    std::string::size_type e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) { s.clear(); return; }
    s = s.substr(b, e - b + 1);
}

void RateTable::load(std::istream& in) {
    std::string line;
    bool header_checked = false;

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // 先頭行がヘッダなら捨てる
        if (!header_checked) {
            header_checked = true;
            std::string t = line; trim(t);
            if (t == "date,exchange_rate") continue;
        }

        // "date,rate" を読む
        std::string::size_type comma = line.find(',');
        if (comma == std::string::npos) continue; // 壊れた行は無視

        std::string date = line.substr(0, comma);
        std::string srate = line.substr(comma + 1);
        trim(date); trim(srate);
        if (date.size() == 0 || srate.size() == 0) continue;

        // rateをdoubleに（末尾にゴミはないと仮定）
        const char* c = srate.c_str();
        char* endp = 0;
        double rate = std::strtod(c, &endp);
        if (endp == c) continue;

        rates_[date] = rate;
    }

    if (rates_.empty())
        throw std::runtime_error("empty rate database.");
}

bool RateTable::getRateForDate(const std::string& date, double& out) const {
    if (rates_.empty()) return false;

    std::map<std::string,double>::const_iterator it = rates_.lower_bound(date);

    out = (it == rates_.end() ? (--it)->second : it->second); 
    return true;  // 同日
}

/////////////////////////// BitcoinExchangeの実装 /////////////////////////////////

static void printError(std::ostream& err, const std::string& msg) {
    err << "Error: " << msg << '\n';
}

void BitcoinExchange::trim(std::string& s) {
    std::string::size_type b = s.find_first_not_of(" \t\r\n");
    std::string::size_type e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) { s.clear(); return; }
    s = s.substr(b, e - b + 1);
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
    this->trim(s);
    return s == "date | value";
}

// YYYY-MM-DD 形式・範囲チェック（うるう年対応）
static bool isLeap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

bool BitcoinExchange::isValidDate(const std::string& d) {
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int y, m, day;  // y -> year, m -> month, day -> day

    if (BitcoinExchange::checkYYYYMMDD(d)) return false;
    BitcoinExchange::split_YYYY_MM_DD(d, y, m, day);
    if (m < 1 || m > 12) return false;
    int maxd = mdays[m-1];
    if (m==2 && isLeap(y)) maxd = 29;
    if (day < 1 || day > maxd) return false;
    return true;
}

// 0〜1000（valueは“between 0 and 1000”）
bool BitcoinExchange::isValidValue(double v) {
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
    bool header_checked = true;

    while (std::getline(input, line, '\n')) {
        try {
            if (line.empty()) continue;

            if (header_checked) {
                header_checked = false;
                if (this->isHeaderLine(line)) continue; // ヘッダは読み飛ばす
            }

            // "date | value"
            std::string::size_type bar = line.find('|');
            if (bar == std::string::npos)
                throw std::exception("bad input => " + line);

            std::string date = line.substr(0, bar);
            std::string sval = line.substr(bar + 1);
            this->trim(date); this->trim(sval); // TODO: trim を std::string の戻り値にする

            if (!this->isValidDate(date))
                throw std::exception("bad input => " + date);

            double val = 0.0;
            if (!this->parseStrictDouble(sval, val))
                throw std::exception("bad input => " + sval);

            if (!this->isValidValue(val))
                throw std::exception(val < 0 ? \
                    "not a positive number." : "too large a number.");

            double rate = 0.0;
            if (!table_.getRateForDate(date, rate))
                throw std::exception("bad input => " + line); // 直近過去が存在しない（DBの最古日より前）

            double result = val * rate;
            out << date << " => " << val << " = " << result << '\n';
        } catch (const std::exception& e) {
            printError(err, e.what());
        }
    }
}

bool BitcoinExchange::checkYYYYMMDD(const std::string& d) {
    bool result = (d.size() == BITCOIN_EXCHANGE_YYYY_MM_DD_END_INDEX);

    for (std::size_t j = 0; j < d.size() && result; j ++) {
        switch (j) {
            case (BITCOIN_EXCHANGE_YYYY_MM_BETWEEN_INDEX):
            case (BITCOIN_EXCHANGE_MM_DD_BETWEEN_INDEX):
                result = (d[j] == '-');
                break;
            default:
                result = std::isdigit(d[j]);
        }
    }
    return result;
}

bool BitcoinExchange::split_YYYY_MM_DD(
      const std::string& d,
      int& y,
      int& m,
      int& day) {
    y = std::atoi(d.substr(0, BITCOIN_EXCHANGE_YYYY_MM_BETWEEN_INDEX).c_str());
    m = std::atoi(d.substr(BITCOIN_EXCHANGE_YYYY_MM_BETWEEN_INDEX + 1, BITCOIN_EXCHANGE_MM_DD_BETWEEN_INDEX).c_str());
    day = std::atoi(d.substr(BITCOIN_EXCHANGE_MM_DD_BETWEEN_INDEX + 1), BITCOIN_EXCHANGE_YYYY_MM_DD_END_INDEX);
}