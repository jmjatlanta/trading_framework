#pragma once

namespace strategy
{

class EvaluationResult
{
   public:
   EvaluationResult();
};

class Event
{

};

class Strategy
{

   public:
   Strategy()
   {

   }
   virtual strategy::EvaluationResult OnPretradeEvent(strategy::Event e ) = 0;
   virtual strategy::EvaluationResult OnCreateOrder(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderSent(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderPartiallyFilled(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderFilled(strategy::Event e) = 0;
   virtual strategy::EvaluationResult OnOrderCanceled(strategy::Event e) = 0;

};

} // namespace strategy