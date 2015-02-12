#include <torrentParser.hpp>
#include <torrent.hpp>
#include <istream>
#include <bencodeParserUtil.hpp>
#include <map>
#include <iostream>

using std::string;
using std::list;


namespace torrent {
    static std::list<string> getAnnounceList(const BencodeList* announceListToken)
    {
        std::list<string> t;
        return std::move(t);
    }
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
        getAnnounceList(dynamic_cast<BencodeList *>(torrentDict["announce-list"]));
        
        return std::move(t);
     }
}

