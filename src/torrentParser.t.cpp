#include "torrentParser.hpp"
#include <sstream>
#include <fstream>

int main() {
//    std::string str = "i123e";
//    std::istringstream in(str);
//    torrent::TorrentParserUtil::parseFile(in);
//    str = "4:spam";
//    in = std::istringstream(str);
//    torrent::TorrentParserUtil::parseFile(in);
//    str = "l4:spami42ee";
//    in = std::istringstream(str);
//    torrent::TorrentParserUtil::parseFile(in);
//    str = "d3:bar4:spam3:fooi42ee";
//    in = std::istringstream(str);
//    torrent::TorrentParserUtil::parseFile(in);
    
    std::ifstream ifs("/Users/admin/Desktop/ubuntu-14.10-desktop-amd64.iso.torrent", std::ifstream::in);
    torrent::TorrentParserUtil::parseFile(ifs);
}