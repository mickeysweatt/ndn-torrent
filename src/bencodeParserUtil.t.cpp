// bencdeParserUtil.t.cpp                                              -*-C++-*-
#include <bencodeParserUtil.hpp>
#include <string>
#include <sstream>

int main()
{
   std::string str = "i123e";
   std::istringstream in(str);
   torrent::BencodeParserUtil::parseStream(in);
   str = "4:spam";
   std::istringstream in2(str);
   torrent::BencodeParserUtil::parseStream(in2);
   str = "l4:spami42ee";
   std::istringstream in3(str);
   torrent::BencodeParserUtil::parseStream(in3);
   str = "d3:bar4:spam3:fooi42ee";
   std::istringstream in4(str);
   torrent::BencodeParserUtil::parseStream(in4);
   return 0;
}
