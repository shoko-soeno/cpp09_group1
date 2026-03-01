#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP
#include <map>
#include <string>
#include <iostream>
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
    explicit BitcoinExchange(std::istream& dbCsv);
    BitcoinExchange(const BitcoinExchange&);
    BitcoinExchange& operator=(const BitcoinExchange& src);
    ~BitcoinExchange();
    void run(std::istream& input, std::ostream& out, std::ostream& err);

    static bool isValidDate(const std::string& date);
    static bool isValidValue(double v);
    static bool isHeaderLine(std::string s);

private:
    /**
      * C++98 には std::stod がないので、strtodを使ってパース
      * @param s 入力文字列
      * @param out 変換結果の出力先
      * @return 変換成功なら true、失敗なら false
    */
    static bool parseDouble(const std::string& sval, double& out);

    static bool checkYMD(const std::string& date);
    static bool splitYMD(
      const std::string& date,
      int& y,
      int& m,
      int& day);
private:
    RateTable table_;
};

#endif // BITCOINEXCHANGE_HPP