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
   in = std::istringstream(str);
   torrent::BencodeParserUtil::parseStream(in);
   str = "l4:spami42ee";
   in = std::istringstream(str);
   torrent::BencodeParserUtil::parseStream(in);
   str = "d3:bar4:spam3:fooi42ee";
   in = std::istringstream(str);
   torrent::BencodeParserUtil::parseStream(in);
}
