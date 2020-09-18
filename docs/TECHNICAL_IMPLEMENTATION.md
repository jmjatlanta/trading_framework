# Technical Details

Here, I will attempt to document my rationale for certain decisions. As the project is very young (premature?), these of course could change drasticly.

For a more general discussion of the project, [click here](../README.md).

## Components vs Microservices

There is a debate in my own head as to wheter certain components would be better served as microservices or components embedded in an application.

My current gut reaction is that components would be a better choice initially. However, it should be kept in mind that as soon as a second strategy is added, things like monitoring accounts and strategies may be easier as microservices instead. Stay tuned (I know I will).

## External Dependencies

As a C++ developer, I enjoy the challenge of building things from scratch. As a human being, I know I am not perfect, and I have not experienced all situations. Therefore, I have learned that certain things are best left to the giants before me.

It would be a great learning experience to implement containers and algorithms available in the STL. But the results I am looking for is a trading framework. Such high-level software is not for review of the standards committee. I'll leave the implementation of such low-level components to such experts in their field.

This project will be using some third-party components beyond the STL. Boost will almost certainly be used, in fact, quite early. I do not want to build a unit testing framework (again).

The above being said, I do not like many external dependencies. The STL, Boost, and probably a TA math library (as yet undecided as to which) will hopefully be the extent of external dependencies.

Should microservices be chosen, I'd probably add ZeroMQ (I have experience) or nanomsg (no experience, but probably a better choice).

## Project Layout

CMake will be used to help with platform and development environment tasks. It is my hope that I can genericize as much as practical to make porting to other platforms possible.

Clean interfaces make many things easier, so up-front effort will be expended on componentizing the systems. For example:

Interactive Brokers provides a popular API. Their provided interface is all-encompasing. I will be breaking it into sub-categories. The goal is to "componentize" their API to specific concentrations. That way, replacing the historical datafeed with another but keeping the order entry and management with Interactive Brokers is made possible.

### Major components

#### Streaming Market Data

This component will allow subscribers to access data that is streaming from the market.

#### Historical Market Data

Many strategies (most of mine) rely on data beyond the current day. This component will provide access to queriable historical data.

#### Account / Risk Management

This will provide for keeping up with account balances, and up-to-the-second (and thread-safe) access to open orders, CAR, and other account-related maths. This will also be the only component that can turn a "proposed trade" into actionable orders.

#### Strategy

This is where the daily work is done. Initially this will probably be in C++, but I would like to make this where Python scripts are provided access to the rest of the system components.

This component may be split into entry strategies and exit strategies, with an over-encompasing strategy on top. I am still hammering out the details.

```
include
src
   |
   -- market_data
      |
      -- historical
      -- streaming
   |
   -- domain
   -- strategy
```

The basic directory structure will be duplicated under the `include` directory for header files. Keeping this organized is always a challenge. Things will certainly move around, especially initially.