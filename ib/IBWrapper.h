/***
 * Interface to Interactive Brokers
 * 
 * Note: Many of these methods are multithreaded request/response, and return std::future
 */
#pragma once

#include "ib_client/StdAfx.h"
#include <thread>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <atomic>
#include <list>
#include <iomanip>

#include "ib_client/EWrapper.h"
#include "ib_client/EReader.h"
#include "ib_client/EClientSocket.h"
#include "ib_client/EReaderOSSignal.h"
#include "ib_client/CommonDefs.h"

#include <market_data/Contract.hpp>
#include <market_data/BarSettings.hpp>
#include <market_data/streaming/StreamingService.hpp>

namespace ib {

class IBException : public std::exception
{
   private:
   std::string msg;
   public:
   IBException(std::string msg) : msg(msg) {};
   virtual std::string what() { return msg; }
};

enum ConnectionStatus {
   UNKNOWN, // class probably newly constructed
   CONNECTING, // in the process of connecting
   CONNECTED, // we are fully connected
   DISCONNECTED // something happened, and we closed the connection
};

std::string SecurityTypeToIBString(tf::SecurityType in);

std::string BarTimeSpanToIBString(tf::BarTimeSpan in);

std::string DurationToIBDuration(std::chrono::system_clock::duration in);

std::string TimeToIBTime(std::chrono::system_clock::time_point tp);

market_data::TickType IBTickTypeToMDTickType(TickType ib_tt);

class TickMessage
{
   public:
   TickType tickType;
   double d;
   long l;
   std::string msg;
};

class IBWrapper: public EWrapper {
public:
	IBWrapper(std::string host, int port, int clientId);
	virtual ~IBWrapper();
private:
	std::string host;
	int port;
	int clientId;
	bool m_extraAuth;
	std::atomic<OrderId> nextValidOrderId;
	std::atomic<ConnectionStatus> connectionStatus{ConnectionStatus::UNKNOWN};
   std::string managed_accounts = ""; // a comma separated string of managed accounts (see managedAccounts method)
	bool connectionError;
	EReader* m_pReader;
	/**
	 * State and storage components
	 */
   std::map<int, std::vector<Bar>> historicalBars;
	 // A list of completed requests
	std::set<int> completedRequests;
protected:
	std::shared_ptr<std::thread> listener = nullptr;
	volatile bool shutting_down;
	EReaderOSSignal* m_osSignal;
	EClientSocket* m_pClient;
	int openOrdersRequestId;
public:
	// Functions to kick off calls to IB and retrieve results
	/**
	 * Determine if we are connected to TWS
    * @returns the connection status
	 */
	ConnectionStatus GetConnectionStatus() { return connectionStatus; }
	/***
	 * Determine if a particular request has been completed
	 */
	bool isCompleted(int reqId) {
		return completedRequests.find(reqId) != completedRequests.end();
	}

   int reqHistoricalData(Contract contract, std::string endDateTime, std::string durationString,
         std::string barSizeSetting, std::string whatToShow, int useRTH, int formatDate, bool keepUpToDate, TagValueListSPtr chartOptions)
   {
      TickerId tickerId = nextValidOrderId++;
      m_pClient->reqHistoricalData(tickerId, contract, endDateTime, durationString, barSizeSetting, whatToShow, useRTH, formatDate, keepUpToDate, chartOptions);
      return tickerId;
   }

   std::vector<Bar> GetHistoricalData(int reqId)
   {
      std::vector<Bar> retVal = historicalBars[reqId];
      historicalBars.erase(reqId);
      return retVal;
   }

   void SubscribeToTickData(const Contract& contract, std::function<void(TickMessage)> func)
   {

   }

	bool requestContractDetails(int requestId, const Contract& contract);
	/***
	 * Requests the fundamental data report
	 */
	int RequestFundamentals(std::string ticker);
	std::string GetFundamentals(int requestId);
	/***
	 * Request open orders
	 */
	int RequestOpenOrders();
	bool requestTicks(int requestId, const Contract& contract);
	/***
	 * Places an order with IB
	 * @param contract the contract
	 * @param order the order
	 * @returns the order ID
	 */
	int PlaceOrder(const Contract& contract, const Order& order);
	// these are used to process messages from IB
	static void process_messages(IBWrapper* ibWrapper);
	void processMessage();

	/**
	 * Basic Company Information
	 */

	/***
	 * Get basic company information. Note this call gets its data
	 * from various calls to TWS
	 * @param ticker the ticker
	 * @returns the request id
	 */
	int RequestCompanyInformation(std::string ticker) {
		Contract contract = getContract(ticker);
		// use that contract to get company details
		int reqId = this->nextValidOrderId++;
		this->m_pClient->reqContractDetails(reqId, contract);
		return reqId;
	}

	/***
	 * Scanner and Watchlist stuff
	 */

	/***
	 * This fires of a scanner
	 * @returns the request id
	 */
	int RequestScannerSubscription();

	/***
	 * Cancels the scanner subscription and deletes the results
	 * from the internal vector of results
	 * @param reqId the request id
	 */
	void CancelScannerSubscription(int reqId);


	/***
	 * OHLC
	 */

   int RequestHistoricalNews(std::string ticker);

   /***
    * @brief request option details
    * 
    * @param underlyingSymbol the underlying
    * @param futFopExchange the exchange where the options are trading (blank for all)
    * @param underlyingSecType the type of the underlying (i.e. "STK")
    * @param underlyingConId the contract id of the underlying security
    * @returns the request id;
    */
   int RequestsSecurityDefinitionOptionalParameters(std::string underlyingSymbol, std::string futFopExchange, 
         std::string underlyingSecType, int underlyingConId);

	// overridden EWrapper functions
   /***
    * @brief The IB response to a request for the next valid order id
    * NOTE: This is automatically called when you connect, as well as when EClient::reqIds is called 
    * @param order_id the ID received from IB
    */
	void nextValidId(OrderId order_id);
	void managedAccounts( const std::string& accountsList);
	void contractDetails(int reqId, const ContractDetails& contractDetails);
	void tickPrice( TickerId tickerId, TickType field, double price, const TickAttrib& attribs);
	void tickSize( TickerId tickerId, TickType field, int size);
	void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
	void tickGeneric(TickerId tickerId, TickType tickType, double value);
	void tickString(TickerId tickerId, TickType tickType, const std::string& value);
	void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
		   double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate);
	void orderStatus( OrderId orderId, const std::string& status, double filled,
		   double remaining, double avgFillPrice, int permId, int parentId,
		   double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice);
	void openOrder( OrderId orderId, const Contract&, const Order&, const OrderState&);
	void openOrderEnd();
	void winError( const std::string& str, int lastError);
	void connectionClosed();
	void updateAccountValue(const std::string& key, const std::string& val,
			const std::string& currency, const std::string& accountName);
	void updatePortfolio( const Contract& contract, double position,
	      double marketPrice, double marketValue, double averageCost,
	      double unrealizedPNL, double realizedPNL, const std::string& accountName);
	void updateAccountTime(const std::string& timeStamp);
	void accountDownloadEnd(const std::string& accountName);
	void bondContractDetails( int reqId, const ContractDetails& contractDetails);
	void contractDetailsEnd( int reqId);
	void execDetails( int reqId, const Contract& contract, const Execution& execution);
	void execDetailsEnd( int reqId);
	void error(const int id, const int errorCode, const std::string& errorString);
	void updateMktDepth(TickerId id, int position, int operation, int side,
	      double price, int size);
	void updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
	      int side, double price, int size, bool isSmartDepth);
	void updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch);
	void receiveFA(faDataType pFaDataType, const std::string& cxml);
	void historicalData(TickerId reqId, const Bar& bar);
	void historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr);
	void scannerParameters(const std::string& xml);
	void scannerData(int reqId, int rank, const ContractDetails& contractDetails,
		   const std::string& distance, const std::string& benchmark, const std::string& projection,
		   const std::string& legsStr);
	void scannerDataEnd(int reqId);
	void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
		   long volume, double wap, int count);
	void currentTime(long time);
	void fundamentalData(TickerId reqId, const std::string& data);
	void deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract);
	void tickSnapshotEnd( int reqId);
	void marketDataType( TickerId reqId, int marketDataType);
	void commissionReport( const CommissionReport& commissionReport);
	void position( const std::string& account, const Contract& contract, double position, double avgCost);
	void positionEnd();
	void accountSummary( int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& curency);
	void accountSummaryEnd( int reqId);
	void verifyMessageAPI( const std::string& apiData);
	void verifyCompleted( bool isSuccessful, const std::string& errorText);
	void displayGroupList( int reqId, const std::string& groups);
	void displayGroupUpdated( int reqId, const std::string& contractInfo);
	void verifyAndAuthMessageAPI( const std::string& apiData, const std::string& xyzChallange);
	void verifyAndAuthCompleted( bool isSuccessful, const std::string& errorText);
	void connectAck();
	void positionMulti( int reqId, const std::string& account,const std::string& modelCode, const Contract& contract, double pos, double avgCost);
	void positionMultiEnd( int reqId);
	void accountUpdateMulti( int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency);
	void accountUpdateMultiEnd( int reqId);
   /***
    * @brief Returns security definition optional parameters (useful for options)
    * 
    * A response to the client's call to reqSecDefOptParams
    * 
    * @param reqId the request identifier
    * @param exchange the exchange
    * @param underlyingConId the underlying contract id
    * @param tradingClass the trading class
    * @param multiplier the multiplier
    * @param expirations the expirations
    * @param strikes the strikes
    */
	void securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass, 
         const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes);
	void securityDefinitionOptionalParameterEnd(int reqId);
	void softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers);
   void familyCodes(const std::vector<FamilyCode>& familyCodes);
   void symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions);
   void mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions);
   void tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, 
      const std::string& headline, const std::string& extraData);
   void smartComponents(int reqId, const SmartComponentsMap& theMap);
   void tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions);
   void newsProviders(const std::vector<NewsProvider> &newsProviders);
   void newsArticle(int requestId, int articleType, const std::string& articleText);
   void historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline);
   void historicalNewsEnd(int requestId, bool hasMore);
   void headTimestamp(int reqId, const std::string& headTimestamp);
   void histogramData(int reqId, const HistogramDataVector& data);
   void historicalDataUpdate(TickerId reqId, const Bar& bar);
   void rerouteMktDataReq(int reqId, int conid, const std::string& exchange);
   void rerouteMktDepthReq(int reqId, int conid, const std::string& exchange);
   void marketRule(int marketRuleId, const std::vector<PriceIncrement> &priceIncrements);
   void pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL);
   void pnlSingle(int reqId, int pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value);
   void historicalTicks(int reqId, const std::vector<HistoricalTick>& ticks, bool done);
   void historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk>& ticks, bool done);
   void historicalTicksLast(int reqId, const std::vector<HistoricalTickLast>& ticks, bool done);
   void tickByTickAllLast(int reqId, int tickType, time_t time, double price, int size, const TickAttribLast& tickAttribLast, const std::string& exchange, const std::string& specialConditions);
   void tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, int bidSize, int askSize, const TickAttribBidAsk& tickAttribBidAsk);
   void tickByTickMidPoint(int reqId, time_t time, double midPoint);
   void orderBound(long long orderId, int apiClientId, int apiOrderId);
   void completedOrder(const Contract& contract, const Order& order, const OrderState& orderState);
   void completedOrdersEnd();

private:
	/***
	 * Build an IB stock Contract based on a ticker
	 * @param ticker the ticker to use
	 * @returns a US stock Contract
	 */
	Contract getContract(std::string ticker) {
		// build a contract
		Contract contract;
		contract.currency = "USD";
		contract.localSymbol = ticker;
		contract.symbol = ticker;
		contract.secType = "STK";
		contract.exchange = "SMART";
		return contract;
	}
};

}
