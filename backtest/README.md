This directory provides interfaces and an engine for backtesting. It should comply with 
typical interfaces (i.e. Interactive Brokers) so that strategies do not need coding
changes to go from testing to live trading.

The hope is that this is the base. I am sure it will require expantion for backtests of any
complexity.

What is here is a mock "state machine" that can take data in a specific format and replay it in rapid-fire. The resolution of the data depends on the resolution of the data. Higher resolution will take more time, and will result in hopefully more accurate backtesting.

It would be nice to be able to go all the way down to tick and order book data. I'm sure we won't go that far initially, but it is a goal.

Idea: A "preload" step to get data that can be asked for via the historical service, and then a "play" step that actually runs the strategy. Perhaps it can be as smart as the strategy telling the system what data it needs, including historical data.

Example: A strategy needs 5 minute bars for D(-1) and D(-2), and tick data for today for both the S&P 500 index and AAPL. The backtester could gather that information from what it has, and know what date ranges it could perform a backtest for. 