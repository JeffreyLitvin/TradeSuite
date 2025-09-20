#include <string>

struct Trade
{
    std::string sym; 
    std::string tradeDate; 
    bool longTrade;
    bool withTrend;
    std::string openDate; 
    int risk;
    int pnl;
    std::string closeDate; 
    std::string tradeType; 
};