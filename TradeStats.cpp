#include <iostream>
#include <fstream>
#include "Trade.hpp"
#include "TradeStats.hpp"
#include <cxxopts/cxxopts.hpp>
#include "csv.h"
#include "ascii/ascii.h"
#include "fort.hpp"
#include <cmath>

double two_decimals(double value) {
    return std::trunc(value * 100.0) / 100.0;
}

int main(int argc, char *argv[])
{
    cxxopts::Options options("TradeStats", "Produces trade stats");
    options.add_options()
        ("f,file", "File name", cxxopts::value<std::string>())
        ("h,help", "Print usage")
        ;
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    std::string file;
    if (result.count("file"))
    {
        file = result["file"].as<std::string>();
    }
    else
    {
        std::cout << "File argument required!" << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    TradeStatManager mgr;
    mgr.readTradeFile(file);

    const auto& stats = mgr.getTradeStats("ALL");
    ascii::Asciichart asciichart({{"ALLTRADEs", mgr.getTradeStats("ALL").getTradesAsSeries()}});
    std::cout << asciichart.Plot() << std::endl;

    fort::char_table table;
    table << fort::header
        << "Trade Type"         << "Total Trades"           << "Win Pct"            << "Total R"                            << fort::endr
        << "ALL"                << stats.getTotalTrades()   << stats.getWinPct()    << two_decimals(stats.getRunningR())    << fort::endr;
    std::cout << table.to_string() << std::endl;

}
