#include <domain/Strategy.hpp>

#include <ib/IBAccountService.hpp>
#include <ib/IBHistoricalService.hpp>
#include <ib/IBStreamingService.hpp>

namespace ib
{

class IBStrategyRunner : public tf::StrategyRunner<IBStrategyRunner>
{
   public:
   IBStrategyRunner(const IBConfiguration& accountConfig, const IBConfiguration& historicalConfig, const IBConfiguration& streamingConfig);

   private:
   IBAccountService accountService;
   IBHistoricalService historicalService;
   IBStreamingService streamingService;
};

} // namespace ib