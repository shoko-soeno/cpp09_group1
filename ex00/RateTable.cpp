#include "RateTable.hpp"
#include "Utils.hpp"

RateTable::RateTable() {}
RateTable::~RateTable() {}
RateTable::RateTable(const RateTable& src) {
    if (this != &src)
        *this = RateTable();
};
RateTable& RateTable::operator=(const RateTable& src){
    if (this != &src) {
        this->rates_ = src.rates_;
    }
    return *this;
}

void RateTable::load(std::istream& in) {
    std::string line;
    bool header_checked = false;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (!header_checked) {
            header_checked = true;
            std::string t = trim(line);
            if (t == "date,exchange_rate") continue; // skip header
        }
        std::string::size_type comma = line.find(',');
        if (comma == std::string::npos) continue; // skip malformed line

        std::string date = trim(line.substr(0, comma));
        std::string srate = trim(line.substr(comma + 1));
        if (date.size() == 0 || srate.size() == 0) continue;

        const char* c = srate.c_str();
        char* endp = 0;
        double rate = std::strtod(c, &endp);
        if (endp == c) continue; // failed to parse
        if (*endp != '\0') continue; // extra characters after number
        rates_[date] = rate;
    }

    if (rates_.empty())
        throw std::runtime_error("empty rate database.");
}

bool RateTable::getRateForDate(const std::string& date, double& out) const {
    if (rates_.empty()) return false;

    // lower_bound returns the first element with key >= date
    std::map<std::string,double>::const_iterator bound_it = rates_.lower_bound(date);

    if (bound_it == rates_.end()) {
        out = (--bound_it)->second;
    } else if (bound_it == rates_.begin() && bound_it->first > date) {
        return false; // no rate available
    } else if (bound_it->first > date) {
        out = (--bound_it)->second;
    } else {
        out = bound_it->second; // exact match
    }
    return true;
}
