 #pragma once

#include <string>
#include <map>
#include <fstream>
#include "csv.h"
#include "Trade.hpp"

class TradeStats
{
    public:

    TradeStats(std::string label) : _label(label)
    {}

    std::string getLabel() const { return _label;}

    void addTrade(Trade t)
    {
        double r = static_cast<double>(t.pnl) / static_cast<double>(t.risk);
        if(r >= 0)
        {
            ++_wins;
        }

        _runningR += r;
        _disciplinedTrades += t.discipline;
        _tradeR.push_back(_runningR);
    }

    int getTotalTrades()   const  { return _tradeR.size();}
    double getRunningR()      const  { return _runningR;}
    int getWins()          const  { return _wins;}
    int getWinPct() const
    {
        return (static_cast<double>(getWins()) / static_cast<double>(getTotalTrades())) * 100;  
    }

    int getDisciplinedPct() const
    {
        return (static_cast<double>(_disciplinedTrades) / static_cast<double>(getTotalTrades())) * 100;  
    }

    const std::vector<double>& getTradesAsSeries() const { return _tradeR;};

    private:
    std::string _label;
    std::vector<double> _tradeR;
    int _wins               = 0;
    int _disciplinedTrades  = 0;
    double _runningR        = 0;

};

class Trades
{
    public:

    Trades(std::string label) : _label(label)
    { }

    std::string getLabel()const { return _label;}

    void addTrade(Trade t)
    {
        _trades.push_back(t);
    }

    TradeStats getStats(int xMostRecent) const
    {
        int ndx = 0;
        if(xMostRecent <= _trades.size())
        {
            ndx = _trades.size() - xMostRecent;
        }

        TradeStats ts(_label);
        for(; ndx < _trades.size(); ++ndx)
        {
            ts.addTrade(_trades.at(ndx));
        }

        return ts;
    }


    private:
    std::string _label;
    std::vector<Trade> _trades;

};

class TradeStatManager
{
    public:

    void readTradeFile(std::string f)
    {
        io::CSVReader<10> in(f);

        int longTrade;
        int withTrend;
        double r;
        Trade t;
        int discipline = 0;

        while (in.read_row(t.sym, t.openDate, longTrade, withTrend, t.tradeType, t.risk, t.closeDate, t.pnl, r, discipline))
        {
            t.longTrade  = longTrade;
            t.withTrend  = withTrend;
            t.discipline = discipline;
            addTrade(t);
        }
    }

    void addTrade(Trade t)
    {
        _allTrades.addTrade(t);

        std::string tradeType;
        if(t.longTrade)
        {
            tradeType += "Long";
        }
        else
        {
            tradeType += "Short";
        }

        if(t.withTrend)
        {
            tradeType += "WithTrend";
        }
        else
        {
            tradeType += "CounterTrend";
        }

        tradeType += t.tradeType;
        auto [it, inserted] = _tradeTypes.try_emplace(tradeType, Trades(tradeType)); 
        it->second.addTrade(t);
    }

    const Trades& getAllTrades()
    {
        return _allTrades;
    }

    template<typename Func>
    void forEachTradeType(Func fn)
    {
        for (const auto& pair : _tradeTypes) 
        {
            fn(pair.second);
        }
    }

    private:
    Trades _allTrades{"ALL"};
    std::map<std::string, Trades> _tradeTypes;
};