// torrentParserUtil.t.cpp                                             -*-C++-*-

#include <torrentParserUtil.hpp>
#include <sstream>
#include <fstream>
#include <vector>

 int main() {
     std::ifstream singleFileTorrent("/Users/admin/Desktop/ubuntu-14.10-desktop-amd64.torrent", std::ios::binary);
     std::ifstream manyFileTorrent("/Users/admin/Desktop/groks113_archive.torrent", std::ios::binary);
     torrent::TorrentParserUtil::parseFile(singleFileTorrent);
     torrent::TorrentParserUtil::parseFile(manyFileTorrent);
     
 }