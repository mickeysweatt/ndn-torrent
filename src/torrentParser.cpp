#include "torrentParser.hpp"
#include "torrent.hpp"
#include <istream>
#include <bencodeParserUtil.hpp>

using std::string;
using std::list;

namespace torrent {

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeToken *ast;
        
        if (in.bad()) {
            throw new ParseError("Bad stream");
        }
        ast = &BencodeParserUtil::parseStream(in);
        return std::move(t);
     }
}

