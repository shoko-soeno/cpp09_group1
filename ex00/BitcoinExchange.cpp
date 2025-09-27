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

    if (it == rates_.end()) {
        // 入力日がDBの最後より未来 → 最後の要素を使う（直近過去）
        --it;
        out = it->second;
        return true;
    }
    if (it->first == date) {
        out = it->second; 
        return true;  // 同日
    }
    // it->first > date → 1つ戻ると直近過去。戻れないなら過去が無い
    if (it == rates_.begin()) return false;
    --it;
    out = it->second;
    return true;
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
        if (*endp!=' ' && *endp!='\t' && *endp!='\r' && *endp!='\n') return false;
        ++endp;
    }
    return true;
}

// ヘッダ "date | value"（前後空白は許容）
bool BitcoinExchange::isHeaderLine(std::string s) {
    trim(s);
    return s == "date | value";
}

// YYYY-MM-DD 形式・範囲チェック（うるう年対応）
static bool isLeap(int y){ return (y%4==0 && y%100!=0) || (y%400==0); }

bool BitcoinExchange::isValidDate(const std::string& d) {
    if (d.size() != 10) return false;
    if (d[4] != '-' || d[7] != '-') return false;
    for (int i=0;i<10;i++) {
        if (i==4 || i==7) continue;
        if (!std::isdigit(static_cast<unsigned char>(d[i]))) return false;
    }
    int y = std::atoi(d.substr(0,4).c_str());
    int m = std::atoi(d.substr(5,2).c_str());
    int day = std::atoi(d.substr(8,2).c_str());
    if (m < 1 || m > 12) return false;
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxd = mdays[m-1];
    if (m==2 && isLeap(y)) maxd = 29;
    if (day < 1 || day > maxd) return false;
    return true;
}

// 0〜1000（課題の“between 0 and 1000”に合わせる）
bool BitcoinExchange::isValidValue(double v) {
    if (v < 0.0) return false;
    if (v > 1000.0) return false;
    return true;
}

void BitcoinExchange::run(std::istream& dbCsv, std::istream& input, std::ostream& out, std::ostream& err) {
    try { table_.load(dbCsv); }
    catch (const std::exception& e) { printError(err, e.what()); return; }

    std::string line;
    bool first = true;

    while (std::getline(input, line)) {
        if (line.empty()) continue;

        if (first) {
            first = false;
            if (isHeaderLine(line)) continue; // ヘッダは読み飛ばす
        }

        // "date | value"
        std::string::size_type bar = line.find('|');
        if (bar == std::string::npos) {
            printError(err, std::string("bad input => ") + line);
            continue;
        }

        std::string date = line.substr(0, bar);
        std::string sval = line.substr(bar + 1);
        trim(date); trim(sval);

        if (!isValidDate(date)) {
            printError(err, std::string("bad input => ") + date);
            continue;
        }

        double val = 0.0;
        if (!parseStrictDouble(sval, val)) {
            printError(err, std::string("bad input => ") + sval);
            continue;
        }

        if (!isValidValue(val)) {
            if (val < 0) printError(err, "not a positive number.");
            else         printError(err, "too large a number.");
            continue;
        }

        double rate = 0.0;
        if (!table_.getRateForDate(date, rate)) {
            // 直近過去が存在しない（DBの最古日より前）
            printError(err, std::string("bad input => ") + line);
            continue;
        }

        double result = val * rate;
        out << date << " => " << val << " = " << result << '\n';
    }
}