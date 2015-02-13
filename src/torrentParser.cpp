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
        list<string> announcelist;
        list<BencodeToken *> tempList1, tempList2;
        BencodeList *curr_list;
        
        if (nullptr == announceListToken) {
            throw TorrentParserUtil::ParseError("announce-list illformatted");
        }
        tempList1 = announceListToken->getTokens();
        //unpack announce-list
        for (auto it : tempList1) {
            curr_list = dynamic_cast<BencodeList *>(it);
            if (nullptr == curr_list) {
                throw TorrentParserUtil::ParseError("announce-list illformatted");
            }
            for (auto it2 : curr_list->getTokens()) {
                tempList2.push_back((it2));
            }
        }
        // grab strings from unpacked list
        for (auto it : tempList2) {
            announcelist.push_back(dynamic_cast<ByteStringToken *>(it)->getString());
        }
        return std::move(announcelist);
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
        announceList = getAnnounceList(dynamic_cast<BencodeList *>(torrentDict["announce-list"]));
        
        return std::move(t);
     }
}

