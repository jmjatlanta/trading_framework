#pragma once
#include <vector>
#include <fstream>
#include <algorithm>
#include <ib/IBHistoricalService.hpp>

class SecurityFile
{
   public:
   SecurityFile(std::string fileName) : fileName(fileName) {}
   ~SecurityFile()
   {
      // write file
      std::ofstream out(fileName);
      std::for_each( lines.begin(), lines.end(), [&out](const market_data::OHLCBar& in){
         out << in.toCSV() << "\n";
      });
      out.close();
   }
   void AddBar(const market_data::OHLCBar& in)
   {
      lines.push_back(in);
   }
   private:
   std::vector<market_data::OHLCBar> lines;
   std::string fileName;
};