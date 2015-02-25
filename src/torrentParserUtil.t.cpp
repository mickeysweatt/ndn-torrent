// torrentParserUtil.t.cpp                                             -*-C++-*-
#include <torrentParserUtil.hpp>
#include <torrent.hpp>

#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>

#define PREFIX "/Users/admin/Dropbox/ndn-torrent/test/"

#include <unistd.h>

 int main() {
     std::ifstream single(PREFIX "ubuntu-14.10-desktop-amd64.torrent",
                          std::ios::binary);
     std::ifstream many(PREFIX "groks113_archive.torrent",
                        std::ios::binary);

     torrent::TorrentParserUtil::parseFile(single);
     torrent::TorrentParserUtil::parseFile(many);
 }