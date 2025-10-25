#include <iostream>
#include <fstream>
#include <limits>   // Required for std::numeric_limits
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
    void trade2CSV(std::ostream& os, const Trade& t);



    public:
    void addTrade();
    void writeTrades(std::string tradeFile);
    std::string getDate(std::string label);

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
        std::stringstream csv;
        trade2CSV(csv, t);
        csv << std::endl;

        outfile   << csv.str();
        std::cout << csv.str();
    }

    std::cout << std::endl;
    std::cout << "Recorded " << _trades.size() << " trades" << std::endl;
}

void MenuMgr::trade2CSV(std::ostream &csv, const Trade &t)
{
        std::stringstream rmultiple;
        rmultiple << std::fixed << std::setprecision(2) << static_cast<double>(t.pnl) / static_cast<double>(t.risk);

        csv       << t.sym << ","
                  << t.openDate << ","
                  << t.longTrade << ","
                  << t.withTrend << ","
                  << t.tradeType << ","
                  << t.risk << ","
                  << t.closeDate << ","
                  << t.pnl << ","
                  << rmultiple.str() << ","
                  << t.discipline; 
}

void MenuMgr::addTrade()
{
    Trade t;

    std::cout << "Symbol: "; std::cin >> t.sym; 
    t.openDate = getDate("OpenDate");
    t.longTrade = isLongTrade();
    t.withTrend = isWithTrend();
    t.tradeType = tradeType();
    std::cout << "Risk: "; std::cin >> t.risk; 
    t.closeDate = getDate("CloseDate");
    std::cout << "PnL: "; std::cin >> t.pnl; 

    bool goodTrade = false;

    CubbyMenu::Menu menu;
    std::stringstream tradecsv;
    trade2CSV(tradecsv, t);
    std::cout << tradecsv.str() << std::endl;
    menu.add_item("Confirm trade", [&goodTrade]() {goodTrade = true;});
    menu.add_item("Discard trade", [&goodTrade]() {goodTrade = false;});
    menu.print();

    if(goodTrade)
    {
        _trades.push_back(t);
    }
}

std::string MenuMgr::getDate(std::string label)
{
    std::stringstream date;


    bool goodDate = false;
    while(!goodDate)
    {
        int i;
        std::cout << label << " year (YYYY) : ";
        std::cin >> i;
        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if(i > 2000 && i < 2025)
        {
            date << i << "-";
            goodDate = true;
        }
    }

    goodDate = false;
    while(!goodDate)
    {
        int i;
        std::cout << label << " month (MM)  : ";
        std::cin >> i;
        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if(i > 0 && i < 13)
        {
            date << std::setw(2) << std::setfill('0') << i << "-";
            goodDate = true;
        }
    }

    goodDate = false;
    while(!goodDate)
    {
        int i;
        std::cout << label << " day (DD)    : ";
        std::cin >> i;
        if(std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if(i > 0 && i < 32)
        {
            date << std::setw(2) << std::setfill('0') << i;
            goodDate = true;
        }
    }


    return date.str();
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