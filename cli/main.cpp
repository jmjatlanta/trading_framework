#include <iostream>
#include <sstream>
#include <fstream>

void syntax(std::string program)
{
   std::cerr << "Syntax: " << program << " [NASDAQ|NYSE]\nPlease assure the symbol file (NYSE.txt or NASDAQ.txt) exists in current directory\n";
   exit(1);
}

std::string add_symbol(std::string csv, std::string symbol)
{
   std::string out = csv.substr(0, csv.find(',') + 1);
   out += symbol;
   out += csv.substr(csv.find(','));
   return out;
}

std::string add_header(std::string csv)
{
   std::string out = csv.substr(0, csv.find(',') + 1);
   out += "Symbol";
   out += csv.substr(csv.find(','));
   return out;
}

std::string get_symbol_file(std::string exchange)
{
   std::string filename = exchange + ".txt";
   std::ifstream in(filename);
   return std::string( (std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>() );
}

std::string chop_symbol(std::string in)
{
   std::string out = in.substr(0, in.find(' '));
   // sometimes there is a tab instead of a space
   if (out.find('\t') != std::string::npos)
      out = out.substr(0, out.find('\t'));
   // don't bother with symbols that have dashes or periods
   if (out.find('.') != std::string::npos || out.find('-') != std::string::npos)
      return "";
   return out;
}

bool is_header(std::string in)
{
   return in.find("Date,") == 0;
}

/***
 * Downloads shorting data from quandl
 */
int main(int argc, char** argv)
{
   if (argc < 2) syntax(argv[0]);

    /* This used to use quandl, but has been removed. This should move to a module
   std::string exchange = argv[1];
   std::string symbol_file = get_symbol_file(exchange);
   if (exchange != "NASDAQ" && exchange != "NYSE") syntax(argv[0]);
   if (exchange == "NASDAQ")
      exchange = "FNSQ_";
   else
      exchange = "FNYX_";
   if (symbol_file.empty())
      syntax(argv[0]);

   //leind::quandl::core q(".csv");
   //q.auth("vbxgzKWu_NgtPHUj1V-C");

   std::stringstream symbol_stream(symbol_file);
   std::string symbol_line;
   std::getline(symbol_stream, symbol_line);
   bool header_printed = false;
   while(true)
   {
      // get next symbol
      if (!std::getline(symbol_stream, symbol_line))
         exit(0);
      // get only the symbol
      std::string symbol = chop_symbol(symbol_line);
      if (symbol.empty())
         continue;
      auto csv = q.request("FINRA/" + exchange + symbol );
      std::istringstream ss(csv);
      std::string line;
      bool isFirst = true;
      while (std::getline(ss, line))
      {
         if (is_header(line))
         {
            if (!header_printed)
            {
               header_printed = true;
               std::cout << add_header(line) << "\n";
            }
         }
         else
         {
            std::cout << add_symbol(line, symbol) << "\n";
         }
      }
   }
    */
   return 0;
}