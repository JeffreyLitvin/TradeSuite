#include <iostream>
#include <fstream>
#include "Trade.hpp"
#include "TradeStats.hpp"
#include <cxxopts/cxxopts.hpp>
#include "csv.h"
#include "ascii/ascii.h"
#include "fort.hpp"
#include <cmath>

std::string two_decimals(double value) {
    std::stringstream str;
    str << std::fixed << std::setprecision(2) << value;

    return str.str(); 
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
    ascii::Asciichart asciichart({{"ALLTRADES", mgr.getTradeStats("ALL").getTradesAsSeries()}});
    std::cout << asciichart.Plot() << std::endl;

    fort::char_table table;
    table << fort::header << "Trade Type" << "Total Trades" << "Win Pct" << "R/Trade" << "Total R"  << fort::endr;
    mgr.forEachTradeType([&table](const TradeStats &stats)
        { 
            if(stats.getTotalTrades() < 10)
            {
                return;
            }
            double rPerTrade = stats.getRunningR() / stats.getTotalTrades();
            table << stats.getLabel() << stats.getTotalTrades() << stats.getWinPct() << two_decimals(rPerTrade) << two_decimals(stats.getRunningR()) << fort::endr;
        });

    std::cout << table.to_string() << std::endl;

}
