/*
 * IBWrapper.cpp
 *
 *  Created on: Oct 2, 2017
 *      Author: JohnJones
 */
#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include "IBWrapper.h"
#include "ib_client/ScannerSubscription.h"
#include "util/SysLogger.h"

namespace ib {

std::string SecurityTypeToIBString(tf::SecurityType in)
{
   switch(in)
   {
      case (tf::SecurityType::STOCK):
         return "STK";
      case (tf::SecurityType::OPTION):
         return "OPT";
      case (tf::SecurityType::FUTURE):
         return "FUT";
      case (tf::SecurityType::BOND):
         return "BOND";
   }
   return "";
}

std::string BarTimeSpanToIBString(tf::BarTimeSpan in)
{
   switch (in)
   {
      case (tf::BarTimeSpan::FIVE_MINUTES):
         return "5 mins";
      case (tf::BarTimeSpan::ONE_DAY):
         return "1 day";
   }
   return "";
}

std::string DurationToIBDuration(std::chrono::system_clock::duration in)
{
   auto hrs = std::chrono::duration_cast<std::chrono::hours>(in).count();
   if (hrs <= 24)
   {
      auto secs = std::chrono::duration_cast<std::chrono::seconds>(in).count();
      return std::to_string(secs) + " S";
   }
   return std::to_string(hrs / 24) + " D";
}

std::string TimeToIBTime(std::chrono::system_clock::time_point tp)
{
   std::time_t tt = std::chrono::system_clock::to_time_t(tp);
   std::tm tm{0};
   gmtime_r(&tt, &tm);
   std::stringstream ss;
   ss << std::put_time(&tm, "%Y%m%d %T %Z");
   return ss.str();
}   

/**
 * Constructor. This class sends and responds to messages from IB
 */
IBWrapper::IBWrapper(std::string host, int port, int clientId) :
	host(host),
	port(port),
	clientId(clientId),
	//m_sleepDeadline(0),
	m_extraAuth(false),
	nextValidOrderId(0),
	connectionStatus(ConnectionStatus::CONNECTING),
	connectionError(false),
	m_pReader(0),
	shutting_down(false),
	openOrdersRequestId(0)
{
	// other initialization here
	m_osSignal = new EReaderOSSignal(1000); // timeout (1000 == 1 sec)
	m_pClient = new EClientSocket(this, m_osSignal);
	if (!m_pClient->eConnect(host.c_str(), port, clientId, m_extraAuth))
		return;
	// message loop
	m_pReader = new EReader(m_pClient, m_osSignal);
	m_pReader->start();
	listener = std::make_shared<std::thread>(process_messages, this);
}

/***
 * Destructor. Clean up after yourself!
 */
IBWrapper::~IBWrapper() {
	shutting_down = true;
   if (listener != nullptr)
	   listener->join();
}

void IBWrapper::process_messages(IBWrapper* ibWrapper) {
	util::SysLogger* logger = util::SysLogger::getInstance();
	while (!ibWrapper->shutting_down) {
		// wait for connection
		if (ibWrapper->connectionStatus != ConnectionStatus::CONNECTED) {
			sleep(1);
		}
		ibWrapper->m_osSignal->waitForSignal();
		if (!ibWrapper->shutting_down)
			ibWrapper->m_pReader->processMsgs();
	}
}

bool IBWrapper::requestContractDetails(int requestId, const Contract& contract) {
	if (connectionError)
		return false;
	//std::cout << "Requesting Contract Details" << std::endl;
	m_pClient->reqContractDetails(requestId, contract);
	return true;
}

int IBWrapper::RequestFundamentals(std::string ticker) {
	if (connectionError)
		return false;
	Contract contract = getContract(ticker);
	long requestId = this->nextValidOrderId++;
   TagValueListSPtr fundamentalDataOptions;
	m_pClient->reqFundamentalData(requestId, contract, "ReportsFinSummary", fundamentalDataOptions);
	return requestId;
}


bool IBWrapper::requestTicks(int requestId, const Contract& contract) {
	if (connectionError)
		return false;
	TagValueListSPtr listSptr;
	//std::cout << "Requesting shorting information" << std::endl;
	m_pClient->reqMktData(requestId, contract, "236", false, false, listSptr);
	return true;
}

/***
 * Requests a subscription to scan for stocks that meet certain criteria
 */
int IBWrapper::RequestScannerSubscription() {
	ScannerSubscription ss;
	ss.instrument = "STK";
	ss.scanCode = "TOP_PERC_GAIN";
	ss.abovePrice = 0.0001;
	ss.aboveVolume = 100000;
	ss.belowPrice = 10;
	ss.numberOfRows = 50;
	int reqId = nextValidOrderId++;
	m_pClient->reqScannerSubscription(reqId, ss, TagValueListSPtr(), TagValueListSPtr());
	return reqId;
}

void IBWrapper::CancelScannerSubscription(int reqId) {
	m_pClient->cancelScannerSubscription(reqId);
}

/***
 * OHLC
 */

/***
 * Requests all open orders
 * @returns a request number
 */
int IBWrapper::RequestOpenOrders() {
	// openOrdersRequestId is negative, so it can be safely stored in the completed set
	int retVal = --openOrdersRequestId;
	m_pClient->reqOpenOrders();
	return retVal;
}

/****
 * @brief Request historical news
 * 
 * This will use historicalNews calls to return the news for the ticker. Default is 1 year and max 300 items
 * @param ticker the desired ticker symbol
 * @returns the request id
 */
int IBWrapper::RequestHistoricalNews(std::string ticker)
{
   int requestId = nextValidOrderId++;
   Contract contract = getContract(ticker);
   std::string providerCodes = ""; // e.g. BZ+FLY
   std::string startDate = ""; // yyyy-MM-dd HH:mm:ss.0 exclusive
   std::string endDate = ""; // yyyy-MM-dd HH:mm:ss.0 inclusive
   int totalResults = 300; // max headlines to fetch (1-300)
   //TagValueList* list = new TagValueList();
   //list->push_back((TagValueSPtr)new TagValue("manual", "1"));
   m_pClient->reqHistoricalNews(requestId, contract.conId, providerCodes, startDate, endDate, totalResults, TagValueListSPtr());
   //TODO: Figure out correct deletion sequence (or if it is even needed)
   //for (TagValueSPtr i : list)
   //   delete i;
   //delete list;
   return requestId;
}

/***
 * @brief request option details
 * 
 * @param underlyingSymbol the underlying
 * @param futFopExchange the exchange where the options are trading (blank for all)
 * @param underlyingSecType the type of the underlying (i.e. "STK")
 * @param underlyingConId the contract id of the underlying security
 * @returns the request id;
 */
int IBWrapper::RequestsSecurityDefinitionOptionalParameters(std::string underlyingSymbol, std::string futFopExchange, 
      std::string underlyingSecType, int underlyingConId)
{
   int reqId = nextValidOrderId++;
   m_pClient->reqSecDefOptParams(reqId, underlyingSymbol, futFopExchange, underlyingSecType, underlyingConId);
   return reqId;
}

int IBWrapper::PlaceOrder(const Contract& contract, const Order& order) {
	int orderId = nextValidOrderId++;
	m_pClient->placeOrder(orderId, contract, order);
	return orderId;
}


/**
 * Capture the nextValidId call
 */
void IBWrapper::nextValidId(OrderId order_id) {
	nextValidOrderId = order_id;
   connectionStatus = ConnectionStatus::CONNECTED;
}

/***
 * Returns a comma separated list of account numbers
 * (i.e. F12345,U135234,U23471)
 * @param accountsList the string
 */
void IBWrapper::managedAccounts( const std::string& accountsList) {
   managed_accounts = accountsList;
   connectionStatus = ConnectionStatus::CONNECTED;
}

void IBWrapper::contractDetails(int reqId, const ContractDetails& contractDetails) {
}


void IBWrapper::tickPrice( TickerId tickerId, TickType field, double price, const TickAttrib& attribs) {
	//std::cout << "tickPrice Ticker ID: " << tickerId << " Field: " << field << " Price: " << price << "<br/>" << std::endl;
}
void IBWrapper::tickSize( TickerId tickerId, TickType field, int size) {
	//std::cout << "tickSize Ticker ID: " << tickerId << " Field: " << field << " Size: " << size << "<br/>" << std::endl;
}
void IBWrapper::tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice) {}
void IBWrapper::tickGeneric(TickerId tickerId, TickType tickType, double value) {
	if (tickType == 46) {
		//company->shortableValue = value;
		completedRequests.insert(tickerId);
	} else {
		//std::cout << "tickGeneric Ticker ID: " << tickerId << " Type: " << tickType << " Value: " << value << "<br/>" << std::endl;
	}
}
void IBWrapper::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
	//std::cout << "tickString Ticker ID: " << tickerId << " Type: " << tickType << " Value: " << value << "<br/>" << std::endl;
}
void IBWrapper::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
	   double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate) {}
void IBWrapper::orderStatus( OrderId orderId, const std::string& status, double filled,
	   double remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice) {}

/***
 * Called when ib->reqOpenOrders() is called
 */
void IBWrapper::openOrder( OrderId orderId, const ::Contract& cont,  const ::Order& ord, const ::OrderState& os) {
}
void IBWrapper::openOrderEnd() {
	completedRequests.insert(openOrdersRequestId);
}
void IBWrapper::winError( const std::string& str, int lastError) {}
void IBWrapper::connectionClosed() {}
void IBWrapper::updateAccountValue(const std::string& key, const std::string& val,
const std::string& currency, const std::string& accountName) {}
void IBWrapper::updatePortfolio( const Contract& contract, double position,
   double marketPrice, double marketValue, double averageCost,
   double unrealizedPNL, double realizedPNL, const std::string& accountName) {}
void IBWrapper::updateAccountTime(const std::string& timeStamp) {}
void IBWrapper::accountDownloadEnd(const std::string& accountName) {}
void IBWrapper::bondContractDetails( int reqId, const ContractDetails& contractDetails) {}
void IBWrapper::contractDetailsEnd( int reqId) {
	completedRequests.insert(reqId);
}
void IBWrapper::execDetails( int reqId, const Contract& contract, const Execution& execution) {}
void IBWrapper::execDetailsEnd( int reqId) {}
void IBWrapper::error(const int id, const int errorCode, const std::string& errorString) {
	if (errorCode == 502)
		connectionError = true;
	if (errorCode != 2104 && errorCode != 2106) {
		std::cerr << "ID: " << id << ", Error Code: " << errorCode << ", " << errorString << std::endl;
	}
	if (id > 0) {
		completedRequests.insert(id);
	}
}
void IBWrapper::updateMktDepth(TickerId id, int position, int operation, int side,
   double price, int size) {}
void IBWrapper::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
   int side, double price, int size, bool isSmartDepth) {}

/***
 * Called when IB sends out a bulletin
 */
void IBWrapper::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {}
void IBWrapper::receiveFA(faDataType pFaDataType, const std::string& cxml) {}
void IBWrapper::historicalData(TickerId reqId, const Bar& bar) {
   historicalBars[reqId].push_back(bar);
}

/***
 * This is supposedly called when all the bars come in. The docs say it exists, but it
 * doesn't exist in the EWrapper.h file. So we'll have to test it
 * @param reqId the request id
 * @param startDateStr the startDate
 * @param endDateStr the end date
 */
void IBWrapper::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {
	completedRequests.insert(reqId);
}

/**
 * an xml of scanner parameters
 */
void IBWrapper::scannerParameters(const std::string& xml) {}
/***
 * scanner data coming in
 */
void IBWrapper::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
	   const std::string& distance, const std::string& benchmark, const std::string& projection,
	   const std::string& legsStr) {
	// get the object
	/*
	std::shared_ptr<pb::Watchlist> list = watchlists[reqId];
	pb::WatchlistItem* item = list->add_item();
	item->set_ticker(contractDetails.summary.symbol);
	*/
}
/***
 * End of scanner data
 */
void IBWrapper::scannerDataEnd(int reqId) {
	completedRequests.insert(reqId);
}
void IBWrapper::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count) {}
void IBWrapper::currentTime(long time) {}
void IBWrapper::fundamentalData(TickerId reqId, const std::string& data) {
}
void IBWrapper::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract) {}
void IBWrapper::tickSnapshotEnd( int reqId) {}
void IBWrapper::marketDataType( TickerId reqId, int marketDataType) {}
void IBWrapper::commissionReport( const CommissionReport& commissionReport) {}
void IBWrapper::position( const std::string& account, const Contract& contract, double position, double avgCost) {}
void IBWrapper::positionEnd() {}
void IBWrapper::accountSummary( int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& curency) {}
void IBWrapper::accountSummaryEnd( int reqId) {}
void IBWrapper::verifyMessageAPI( const std::string& apiData) {}
void IBWrapper::verifyCompleted( bool isSuccessful, const std::string& errorText) {}
void IBWrapper::displayGroupList( int reqId, const std::string& groups) {}
void IBWrapper::displayGroupUpdated( int reqId, const std::string& contractInfo) {}
void IBWrapper::verifyAndAuthMessageAPI( const std::string& apiData, const std::string& xyzChallange) {}
void IBWrapper::verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText) {}
void IBWrapper::connectAck() {}
void IBWrapper::positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, double pos, double avgCost) {}
void IBWrapper::positionMultiEnd( int reqId) {}
void IBWrapper::accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency) {}
void IBWrapper::accountUpdateMultiEnd( int reqId) {}
void IBWrapper::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, 
      const std::string& tradingClass, const std::string& multiplier, const std::set<std::string>& expirations, 
      const std::set<double>& strikes) {}
void IBWrapper::securityDefinitionOptionalParameterEnd(int reqId) {}
void IBWrapper::softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers) {}
void IBWrapper::familyCodes(const std::vector<FamilyCode>& familyCodes) {}
void IBWrapper::symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions) { }
void IBWrapper::mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions) { }
/***
 * Called when news arrives
 */
void IBWrapper::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, 
      const std::string& headline, const std::string& extraData) {}
void IBWrapper::smartComponents(int reqId, const SmartComponentsMap& theMap) { }
void IBWrapper::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions) { }
/***
 * Response to client->reqNewsProviders(), contains available news services
 */
void IBWrapper::newsProviders(const std::vector<NewsProvider> &newsProviders) { }
/****
 * Details of a news article
 */
void IBWrapper::newsArticle(int requestId, int articleType, const std::string& articleText) { }
/**
 * Past news item headers, response to client->reqHistoricalNews
 */
void IBWrapper::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline) { }
/***
 * All news for this request has been sent
 */
void IBWrapper::historicalNewsEnd(int requestId, bool hasMore) { }

void IBWrapper::headTimestamp(int reqId, const std::string& headTimestamp) { }
void IBWrapper::histogramData(int reqId, const HistogramDataVector& data) { }
void IBWrapper::historicalDataUpdate(TickerId reqId, const Bar& bar) { }
void IBWrapper::rerouteMktDataReq(int reqId, int conid, const std::string& exchange) { }
void IBWrapper::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange) { }
void IBWrapper::marketRule(int marketRuleId, const std::vector<PriceIncrement> &priceIncrements) { }
void IBWrapper::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL) { }
void IBWrapper::pnlSingle(int reqId, int pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value) { }
void IBWrapper::historicalTicks(int reqId, const std::vector<HistoricalTick>& ticks, bool done) { }
void IBWrapper::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk>& ticks, bool done) { }
void IBWrapper::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast>& ticks, bool done) { }
void IBWrapper::tickByTickAllLast(int reqId, int tickType, time_t time, double price, int size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions) { }
void IBWrapper::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, int bidSize, int askSize, const TickAttribBidAsk& tickAttribBidAsk) { }
void IBWrapper::tickByTickMidPoint(int reqId, time_t time, double midPoint) { }
void IBWrapper::orderBound(long long orderId, int apiClientId, int apiOrderId) { }
void IBWrapper::completedOrder(const Contract& contract, const Order& order, const OrderState& orderState) { }
void IBWrapper::completedOrdersEnd() { }
} // namespace ib
