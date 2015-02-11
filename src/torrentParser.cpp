#include <torrentParser.hpp>
#include <torrent.hpp>
#include <istream>
#include <bencodeParserUtil.hpp>
#include <map>
#include <iostream>

using std::string;
using std::list;


namespace torrent {

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeDict *ast;
        std::list<string> announceList;
        std::map<ByteStringToken, BencodeToken*, BencodeDict::BencodeDictComparator> torrentDict;
        
        if (in.bad()) {
            throw new ParseError("Bad stream");
        }
        ast = dynamic_cast<BencodeDict *>(&BencodeParserUtil::parseStream(in));
        if (nullptr == ast) {
            throw new ParseError("Illformed torrent file");
        }
        torrentDict = ast->getValues();
        BencodeToken *currTok = torrentDict["announce"];
        if (nullptr == currTok) {
            throw new ParseError("Illformed torrent file");
        }
        return std::move(t);
     }
}

