#include <iostream>
#include <fstream>
#include "Trade.hpp"
#include "TradeStats.hpp"
#include <cxxopts/cxxopts.hpp>
#include "csv.h"
#include "ascii/ascii.h"
#include "fort.hpp"
#include <cmath>
#include <set>

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
        ("a,all", "Show all trades")
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

    bool showAll = result.count("all");

    TradeStatManager mgr;
    mgr.readTradeFile(file);

    const auto& stats = mgr.getAllTrades().getStats(150);
    ascii::Asciichart asciichart({{"ALLTRADES", stats.getTradesAsSeries()}});
    std::cout << asciichart.Plot() << std::endl;

    int ddCount = stats.getDrawDownTradeCount();
    if(ddCount > 0)
    {
        std::cout << "Drawdown: " << ddCount << " trades / "<< two_decimals(stats.getDrawDown()) << "R" << std::endl;
    }

    auto tsCompare = [](const TradeStats &a, const TradeStats &b)
    {
        double aR = a.getRunningR() / a.getTotalTrades();
        double bR = b.getRunningR() / b.getTotalTrades();

        if(aR != bR)
        {
            return aR > bR;
        }

        if(a.getRunningR() != b.getRunningR())
        {
            return a.getRunningR() > b.getRunningR();
        }

        return a.getLabel() < b.getLabel();
    };

    fort::char_table table;
    table << fort::header << "Trade Type" << "Total Trades" << "Discipline Pct" << "Win Pct" << "R/Trade" << "Total R"  << fort::endr;

    {
        const auto &allTrades = mgr.getAllTrades();
        double rPerTrade = stats.getRunningR() / stats.getTotalTrades();
        table << allTrades.getLabel() << stats.getTotalTrades() << stats.getDisciplinedPct() <<stats.getWinPct() << two_decimals(rPerTrade) << two_decimals(stats.getRunningR()) << fort::endr;
    }

    std::set<TradeStats, decltype(tsCompare)> tsSet(tsCompare);

    
    mgr.forEachTradeType([showAll, &tsSet](const Trades &t)
        {
            TradeStats ts = t.getStats(20);
            if (showAll || ts.getTotalTrades() >= 10)
            {
                tsSet.insert(ts);
            }
        });
    
    for(const auto& ts : tsSet)
    {
        double rPerTrade = ts.getRunningR() / ts.getTotalTrades();
        table << ts.getLabel() << ts.getTotalTrades() << ts.getDisciplinedPct() << ts.getWinPct() << two_decimals(rPerTrade) << two_decimals(ts.getRunningR()) << fort::endr;
    }

    std::cout << table.to_string() << std::endl;
}
