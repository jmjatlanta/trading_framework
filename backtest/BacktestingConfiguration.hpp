#pragma once

namespace backtest
{

enum FileType { TICK, BAR };

class BacktestingConfiguration
{
   public:
   std::string fileName;
   FileType fileType;
   double startingCapital = 0.0;
};

} // namespace backtest