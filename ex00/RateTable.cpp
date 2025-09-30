#include "RateTable.hpp"
#include "Utils.hpp"

RateTable::RateTable() {}
RateTable::~RateTable() {}
RateTable::RateTable(const RateTable&) = default;
RateTable& RateTable::operator=(const RateTable&) = default;

void RateTable::load(std::istream& in) {
    std::string line;
    bool header_checked = false;

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // 先頭行がヘッダなら捨てる
        if (!header_checked) {
            header_checked = true;
            std::string t = trim(line);
            if (t == "date,exchange_rate") continue;
        }

        // "date,rate" を読む
        std::string::size_type comma = line.find(',');
        if (comma == std::string::npos) continue; // 壊れた行は無視

        std::string date = trim(line.substr(0, comma));
        std::string srate = trim(line.substr(comma + 1));
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
        // If date is after all keys, use the last rate
        out = (--it)->second;
    } else if (it == rates_.begin() && it->first > date) {
        // If date is before all keys, no rate available
        return false;
    } else if (it->first > date) {
        // Use the previous rate
        out = (--it)->second;
    } else {
        // Exact match or closest previous date
        out = it->second;
    }
    return true;
}
