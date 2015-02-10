//
//  BencodeParser.t.cpp
//  Torrent
//
//  Created by admin on 2/10/15.
//  Copyright (c) 2015 admin. All rights reserved.
//
#include <bencodeParser.hpp>
#include <string>
#include <sstream>

int main()
{
    std::string str = "i123e";
    std::istringstream in(str);
    torrent::BencodeParser::parseStream(in);
    str = "4:spam";
    in = std::istringstream(str);
    torrent::BencodeParser::parseStream(in);
    str = "l4:spami42ee";
    in = std::istringstream(str);
    torrent::BencodeParser::parseStream(in);
    str = "d3:bar4:spam3:fooi42ee";
    in = std::istringstream(str);
    torrent::BencodeParser::parseStream(in);
}
