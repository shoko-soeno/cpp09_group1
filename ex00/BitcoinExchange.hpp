#pragma once
#include <map>
#include <string>
#include <istream>

#define DATE_YEAR_END  4  // YYYY-MM-DD
                          //     ^ <----- ここ
#define DATE_MONTH_END 7  // YYYY-MM-DD
                          //        ^ <-- ここ
#define DATE_TOTAL_LEN 10

// data.csv の読み込みとレート取得を担当するクラス
class RateTable {
public:
    RateTable();
    RateTable(const RateTable&);
    RateTable& operator=(const RateTable&);
    ~RateTable();
  /**
    * Loads exchange rates from the given input stream.
    * The input should be in the format "date,rate" per line.
    * @param in The input stream to read from.
  */
  void load(std::istream& in);
  /**
   * Gets the rate for the given date, or the closest previous date if not found.
   * @param date The date string in YYYY-MM-DD format.
   * @param out Reference to double where the rate will be stored.
   * @return true if a rate was found, false otherwise.
  */
  bool getRateForDate(const std::string& date, double& out) const; // 同日 or 直近過去
private:
  std::map<std::string,double> rates_;
};

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
    /**
      * 文字列の前後の空白を削除する
      * @param s 入力文字列
    */
    static std::string trim(const std::string& s);

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
