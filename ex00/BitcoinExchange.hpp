#pragma once
#include <map>
#include <string>
#include <istream>

// data.csv の読み込みとレート取得を担当するクラス
class RateTable {
public:
  void load(std::istream& in);                // "date,rate" を読む
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

private:
    RateTable table_;
};
