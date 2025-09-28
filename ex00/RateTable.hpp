#ifndef RATETABLE_HPP
#define RATETABLE_HPP
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

#endif // RATETABLE_HPP