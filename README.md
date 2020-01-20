# The Trading Framework
## A framework for building trading systems

### First off - What this system is not (yet?)

This is not a fully developed trading system. I plan to build this out, but it is not at all in a usable state.

While it is my intention that this system will work well for medium-scale strategies, it is not my intention to concentrate on HFT strategies. Adding specialized hardware, tweaking of L1/L2 cache, hot codepaths, etc. is very environment-specific, and will be outside the scope of this project (although such projects would be fun segways).

This README will probably later be moved to make room for real documentation (dependencies, compilation, installation). But for now, I'll let this be the introduction to the project.

I have built a number of trading systems, and would like to standardize some of the mundane tasks so that the concentration can be on the execution of the strategy instead of anciliary activities. In addition, those anciliary activities can be tested, audited and improved for the benefit of all past, current, and future strategies.

### The Situation

There are components that most trading systems have, or at least should have. Some existing frameworks provide these for "free" as part of the platform. I am thinking of platforms like TradeStation and MT4. Making a trade, adding money management rules, etc. are very easy on such platforms. But leaving those platforms (and sometimes simply switching brokers) can be difficult, as sections (or even the whole thing) need adjusting or rewritten.

### The Weapons

I have built a variety of trading systems, and know the basics of what is needed. C++ will be the language of choice for its "don't pay for what you don't use" mantra, its performance, and portability. Many systems have been and are being written in Python, and this will be embedded within the strategy area to allow its use if desired.

### The Objectives 

I will attempt to build (or at least itterate towards) clean interfaces that provide components or microservices. They in turn will provide developed strategies the underlying systems they need to determine if conditions are good to take action. Front-end systems will also be built to provide the end-user a "dashboard" to monitor systems, strategies, and accounts.

### The Tactics

Modularize the underlying systems and concentrate on building the components that work well themselves, and interface well with others.