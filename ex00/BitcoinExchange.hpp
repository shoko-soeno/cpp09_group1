#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP
#include <map>
#include <string>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <cctype>     // isdigit
#include <cstdlib>    // strtod
#include <iomanip>
#include "RateTable.hpp"

// input.txtの解析と検証も含む
class BitcoinExchange {
public:
    BitcoinExchange();
    BitcoinExchange(const BitcoinExchange&);
    BitcoinExchange& operator=(const BitcoinExchange&);
    ~BitcoinExchange();
    void run(std::istream& dbCsv, std::istream& input, std::ostream& out, std::ostream& err);

    // テストしやすいように publicにしておく
    static bool isValidDate(const std::string& date);
    static bool isValidValue(double v);
    static bool isHeaderLine(std::string s);

private:
    /**
      * C++98 には std::stod がないので、strtodを使って厳密にパースする
      * @param s 入力文字列
      * @param out 変換結果の出力先
      * @return 変換成功なら true、失敗なら false
    */
    static bool parseStrictDouble(const std::string& s, double& out);

    static bool checkYYYYMMDD(const std::string& d);
    static bool split_YYYY_MM_DD(
      const std::string& d,
      int& y,
      int& m,
      int& day);
private:
    RateTable table_;
};

#endif // BITCOINEXCHANGE_HPP