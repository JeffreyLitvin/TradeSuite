#pragma once

#include <string>
#include <vector>

struct Trade
{
    std::string sym; 
    bool longTrade;
    bool withTrend;
    std::string openDate; 
    int risk;
    int pnl;
    std::string closeDate; 
    std::string tradeType; 
    bool discipline = false;
};
