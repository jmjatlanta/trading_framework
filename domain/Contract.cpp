#include <domain/Contract.hpp>

namespace tf
{

bool operator < (const Contract& lhs, const Contract& rhs)
{
   return lhs.ticker < rhs.ticker;
}

} // namespace market_data