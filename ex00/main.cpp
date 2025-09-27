#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib> // strtod (C++98 には std::stod がない)

static void printError(const std::string& msg){
    std::cerr << "Error: " << msg << '\n';
}

int main(int argc, char** argv) {
    if (argc != 2) { printError("could not open file."); return 1; }

    // 課題付属CSVを "data.csv" と仮定
    std::ifstream db("data.csv");
    if (!db) { printError("could not open data.csv"); return 1; }

    std::ifstream in(argv[1]);
    if (!in) { printError("could not open file."); return 1; }

    BitcoinExchange app;
    app.run(db, in, std::cout, std::cerr);
    return 0;
}