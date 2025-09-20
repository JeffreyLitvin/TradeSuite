#include <iostream>
#include "CubbyMenu.hpp"
#include "Trade.hpp"
#include <cxxopts/cxxopts.hpp>

#include <ctime>

int main(int argc, char *argv[])
{
    cxxopts::Options options("TradeEntry", "Enter trade program");
    options.add_options()
        ("f,file", "File name", cxxopts::value<std::string>())
        ("s,sym", "Symbol", cxxopts::value<std::string>())
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

    std::string sym;
    if (result.count("sym"))
    {
        sym = result["sym"].as<std::string>();
    }
    else
    {
        std::cout << "sym argument required!" << std::endl;
        std::cout << options.help() << std::endl;
        exit(0);
    }

    CubbyMenu::Menu menu;
    menu.add_header("TradeEntry");

    bool loop = true;

    menu.add_item("Add a trade", []() { std::cout << "Implment enter a trade!\n"; });
    menu.add_item("Exit", [&loop]() { loop = false; });

    while(loop)
    {
        menu.print();
    }

     std::time_t currentTime = std::time(0); 

    // Convert the time_t object to a tm structure for local time
    std::tm* localTime = std::localtime(&currentTime);

    // Extract date components from the tm structure
    int year = localTime->tm_year + 1900; // tm_year is years since 1900
    int month = localTime->tm_mon + 1;    // tm_mon is months since January (0-11)
    int day = localTime->tm_mday;         // tm_mday is day of the month (1-31)

    Trade t;
    t.tradeDate = std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day);
    t.sym       = "NVDA";
    t.openDate  = "2025-09-15";
    t.longTrade = true;
    t.withTrend = true;
    t.tradeType = "TRBO";
    t.risk      = 2100;
    t.closeDate = "2025-09-19";
    t.pnl       = 1500;

    std::stringstream rmultiple;
    rmultiple << std::fixed << std::setprecision(2) << static_cast<double>(t.pnl) / static_cast<double>(t.risk);

    std::cout << t.tradeDate << "," 
              << t.sym  << "," 
              << t.openDate  << "," 
              << t.longTrade  << "," 
              << t.withTrend  << "," 
              << t.tradeType  << "," 
              << t.risk  << "," 
              << t.closeDate  << "," 
              << t.pnl  << "," 
              << rmultiple.str() 
              << std::endl;
}
