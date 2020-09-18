#include <backtest/BacktestStrategyRunner.hpp>
#include <util/CSVParser.hpp>

namespace backtest
{

BacktestStrategyRunner::BacktestStrategyRunner(const BacktestingConfiguration& config)
   : config(config)
{
   streamingService = BacktestStreamingService(config);
   result.available_capital = config.startingCapital;
}

std::chrono::time_point<std::chrono::system_clock> parseTime(std::string in)
{
   std::stringstream ss(in);
   std::chrono::system_clock::time_point tp;
   ss >> date::parse("%Y-%m-%d %H:%M:%S", tp);
   return tp;
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
      if (config.fileType == FileType::BAR)
      {
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
      else // TICK file
      {
         market_data::TickMessage msg(market_data::TickType::LAST, std::atof(parser.elementAt(1).c_str()), 
               std::atol(parser.elementAt(2).c_str()), "Tick");
         tick_func(msg);
      }
   }
   // close the market
   tf::Event e;
   e.eventType = tf::EventType::MARKET_CLOSE;
   std::for_each(strategies.begin(), strategies.end(), [&](std::shared_ptr<tf::Strategy<BacktestHistoricalService>> strategy) {
      process_event(strategy, e);
   });
}

void BacktestStrategyRunner::process_event(std::shared_ptr<tf::Strategy<BacktestHistoricalService>> strategy, const tf::Event& e)
{
   tf::EvaluationResult r = strategy->OnPretradeEvent(e);
   if (r == tf::EvaluationResult::PASSES_EVALUATION)
   {
      tf::Order order;
      r = strategy->OnCreateOrder(e, order);
      if (r == tf::EvaluationResult::PASSES_EVALUATION)
      {
         // do we have the capital?
         if (result.available_capital < order.num_shares * order.limit_price)
         {
            r = strategy->OnOrderCanceled(e, order);
         }
         else
         {
            // place the order
            ++result.num_orders;
            if (order.going_long)
               result.available_capital -= order.num_shares * order.limit_price;
            else
               result.available_capital += order.num_shares * order.limit_price;
            // is this a peak of CAR?
            double used_capital = config.startingCapital - result.available_capital;
            if (used_capital > result.max_car)
               result.max_car = used_capital;
            r = strategy->OnOrderSent(e, order);
            if (r != tf::EvaluationResult::PASSES_EVALUATION)
            {
               r = strategy->OnOrderCanceled(e, order);
            }
            else
            {
               ++result.num_trades;
               if (order.going_long)
               {
                  result.shares_traded_long += order.num_shares;
                  result.current_num_shares += order.num_shares;
               }
               else
               {
                  result.shares_traded_short += order.num_shares;
                  result.current_num_shares -= order.num_shares;
               }
               Trade trade;
               trade.buy = order.going_long;
               // need to include time
               trade.limit_price = order.limit_price;
               trade.num_shares = order.num_shares;
               result.trades.push_back(trade);
               // Alert the strategy that the trade was filled
               r = strategy->OnOrderFilled(e, order);
            }
         }  
      }
   }

}

void BacktestStrategyRunner::tick_func(const market_data::TickMessage& message)
{
   for_each(strategies.begin(), strategies.end(), [&](std::shared_ptr<tf::Strategy<BacktestHistoricalService>> strategy) {
         tf::Event e;
         e.eventType = tf::EventType::LAST;
         e.price = message.price;
         process_event(strategy, e);
   }); // for_each strategy
} // tick_func

} // namespace backtest