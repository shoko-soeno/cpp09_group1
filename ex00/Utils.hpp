#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <ostream>

inline std::string trim(const std::string& s) {
    std::string::size_type b = s.find_first_not_of(" \t\r\n");
    std::string::size_type e = s.find_last_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, e - b + 1);
}

inline void printError(std::ostream& err, const std::string& msg) {
    err << "Error: " << msg << '\n';
}

#endif

// std::string BitcoinExchange::trim(const std::string& s) {
//     std::string::size_type b = s.find_first_not_of(" \t\r\n");
//     std::string::size_type e = s.find_last_not_of(" \t\r\n");
//     if (b == std::string::npos) { return ""; }
//     return s.substr(b, e - b + 1);
// }
