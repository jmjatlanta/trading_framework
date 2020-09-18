# Notes specific to Interactive Brokers

## Update 18-August-2020
The TWS front-end is split into 3 major pieces. The order management service (OMS), the historical data service, and the streaming service. Order numbers will be unique, as they must be requested first from the OMS.

## Singleton vs Multiple

I struggle with this dilema. Having the connection to the Ineteractive Brokers TWS/Gateway could work where one connection handles all communications. However, that means that firing up another system that connects to TWS or the Gateway could mess up order numbering with potentially serious consequences. I do not think that is the right way to go.

Having multiple connections to TWS makes things safer, but now the latest order number must be requested before the order can be placed. That will cause a delay, and I have yet to test how big of one. I still believe this is the safer route, and may be of minimal impact.

## Generic Interfaces

I am attempting to separate functionality, and hide complexity behind an interface (API if you will). This is to make things easier to be replaced. If you want to use IQFeed for incoming data but place orders on IB, I am hoping to make that possible. Such an interface makes things harder, and may limit advanced functionality provided by vendors, but I am hoping that will be minimal. If it becomes a problem, some "reach through" mechanism may come into play.