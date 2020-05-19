
namespace ib
{

/**
 * This should connect to external systems and manage everything related to a strategy
 */
IBStrategyRunner::IBStrategyRunner(const IBConfiguration& accountConfig, const IBConfiguration& historicalConfig, const IBConfiguration& streamingConfig)
      : accountService( accountConfig ), historicalService(historicalConfig), streamingService(streamingConfig)
{
}

} // namespace ib