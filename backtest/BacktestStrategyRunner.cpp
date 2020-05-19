#include <backtest/BacktestStrategyRunner.hpp>
#include <util/CSVParser.hpp>

namespace backtest
{

BacktestStrategyRunner::BacktestStrategyRunner(const BacktestingConfiguration& config)
      : config(config)
{
      streamingService = BacktestStreamingService(config);
}

std::chrono::time_point<std::chrono::system_clock> parseTime(std::string in)
{
      std::tm tm = {};
      std::stringstream ss(in);
      ss >> std::get_time(&tm, "%Y%m%d %H:%M:%S");
      return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

int BacktestStrategyRunner::start()
{
   std::ifstream file(config.fileName);
   std::string line;
   while ( std::getline(file, line) )
   {
      // parse line
      // Time, Open, High, Low, Close, volume
      CSVParser parser(line, ",");
      std::string start_time = parser.elementAt(0);
      // Advance the clock to this time
      accountingService.AdvanceTime(parseTime(start_time));
      int vol = std::atoi(parser.elementAt(5).c_str());
      market_data::TickMessage msg(market_data::TickType::LAST, 
            std::atof(parser.elementAt(1).c_str()), vol / 3 + 1, "Open");
      tick_func(msg);
      msg = market_data::TickMessage(market_data::TickType::LAST, 
            std::atof(parser.elementAt(2).c_str()), vol / 3 + 1, "High");
      tick_func(msg);
      msg = market_data::TickMessage(market_data::TickType::LAST, 
            std::atof(parser.elementAt(3).c_str()), vol / 3 + 1, "Low");
      tick_func(msg);
      msg = market_data::TickMessage(market_data::TickType::LAST, 
            std::atof(parser.elementAt(4).c_str()), vol / 3 + 1, "Close");
      tick_func(msg);
   }
}

void BacktestStrategyRunner::tick_func(const market_data::TickMessage& message)
{
   for_each(strategies.begin(), strategies.end(), [&message](std::shared_ptr<strategy::Strategy<BacktestHistoricalService>> strategy) {
         strategy::Event e;
         e.eventType = strategy::EventType::LAST;
         e.price = message.price;
         // TODO: Do something with the return type
         strategy->OnPretradeEvent(e);
   });
}

} // namespace backtest