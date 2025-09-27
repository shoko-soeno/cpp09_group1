#pragma once
#include <map>
#include <string>
#include <istream>

#define BITCOIN_EXCHANGE_YYYY_MM_BETWEEN_INDEX 4  // YYYY-MM-DD
                                                  //     ^ <----- ここ
#define BITCOIN_EXCHANGE_MM_DD_BETWEEN_INDEX   7  // YYYY-MM-DD
                                                  //        ^ <-- ここ
#define BITCOIN_EXCHANGE_YYYY_MM_DD_END_INDEX  10

// data.csv の読み込みとレート取得を担当するクラス
class RateTable {
public:
  void load(std::istream& in);                // "date,rate" を読む
  /**
   * @param date 
   * @param out  
   *
   * @return [true => ] / [false => ]
   */
  bool getRateForDate(const std::string& date, double& out) const; // 同日 or 直近過去
private:
  std::map<std::string,double> rates_;
};

// input.txtの解析と検証も含む
class BitcoinExchange {
public:
    void run(std::istream& dbCsv, std::istream& input, std::ostream& out, std::ostream& err);

    // テストしやすいように publicにしておく
    static bool isValidDate(const std::string& date);
    static bool isValidValue(double v);
    static bool isHeaderLine(std::string s);

private:
    static void trim(std::string& s);
    static bool parseStrictDouble(const std::string& s, double& out);

    static bool checkYYYYMMDD(const std::string& d);
    static void split_YYYY_MM_DD(
      const std::string& d,
      int& y,
      int& m,
      int& day);
private:
    RateTable table_;
};
