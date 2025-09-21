#include <iostream>
#include <fstream>
#include <vector>
#include "CubbyMenu.hpp"
#include "Trade.hpp"
#include <cxxopts/cxxopts.hpp>

#include <ctime>

void addTrade();

class MenuMgr
{
    private:
    std::vector<Trade> _trades;
    bool isLongTrade();
    bool isWithTrend();
    std::string tradeType();

    public:
    void addTrade();
    void writeTrades(std::string tradeFile);

};

int main(int argc, char *argv[])
{
    cxxopts::Options options("TradeEntry", "Enter trade program");
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

    CubbyMenu::Menu menu;
    menu.add_header("TradeEntry");

    MenuMgr m;
    bool loop = true;

    menu.add_item("Add a trade", [&m]() {m.addTrade();});
    menu.add_item("Exit", [&loop]() { loop = false; });

    while(loop)
    {
        menu.print();
    }

    m.writeTrades(file);
}

void MenuMgr::writeTrades(std::string file)
{
    std::ofstream outfile(file, std::ios::app);

    // Check if the file was successfully opened
    if (!outfile.is_open())
    {
        std::cerr << "Unable to open file " << file <<  std::endl;
        return;
    }

    for(Trade t : _trades)
    {
        std::stringstream rmultiple;
        rmultiple << std::fixed << std::setprecision(2) << static_cast<double>(t.pnl) / static_cast<double>(t.risk);

        std::stringstream csv;
        csv       << t.sym << ","
                  << t.openDate << ","
                  << t.longTrade << ","
                  << t.withTrend << ","
                  << t.tradeType << ","
                  << t.risk << ","
                  << t.closeDate << ","
                  << t.pnl << ","
                  << rmultiple.str()
                  << std::endl;

        outfile   << csv.str();
        std::cout << csv.str();
    }

    std::cout << std::endl;
    std::cout << "Recorded " << _trades.size() << " trades" << std::endl;
}

void MenuMgr::addTrade()
{
    Trade t;

    std::cout << "Symbol: "; std::cin >> t.sym; 
    std::cout << "OpenDate (YYYY-MM-DDDD): "; std::cin >> t.openDate; 
    t.longTrade = isLongTrade();
    t.withTrend = isWithTrend();
    t.tradeType = tradeType();
    std::cout << "Risk: "; std::cin >> t.risk; 
    std::cout << "CloseDate (YYYY-MM-DDDD): "; std::cin >> t.closeDate; 
    std::cout << "PnL: "; std::cin >> t.pnl; 

    _trades.push_back(t);
}

bool MenuMgr::isLongTrade()
{
    CubbyMenu::Menu menu;
    bool isLong;
    menu.add_item("Long", [&isLong]() {isLong = true;});
    menu.add_item("Short", [&isLong]() {isLong = false;});
    menu.print();

    return isLong;
}

bool MenuMgr::isWithTrend()
{
    CubbyMenu::Menu menu;
    bool b;
    menu.add_item("With trend", [&b]() {b = true;});
    menu.add_item("Counter trend", [&b]() {b = false;});
    menu.print();

    return b;
}

std::string MenuMgr::tradeType()
{
    CubbyMenu::Menu menu;
    std::string s;
    menu.add_item("Trading Range Break Out", [&s]() {s = "TRBO";});
    menu.add_item("Trading Range Buy Low Sell High", [&s]() {s = "TRBLSH";});
    menu.add_item("Broad Channel Break Out", [&s]() {s = "BCBO";});
    menu.add_item("Broad Channel Buy Low Sell High", [&s]() {s = "BCBLSH";});
    menu.add_item("Major/Minor Trend Reversal", [&s]() {s = "MTR";});
    menu.print();

    return s;
}