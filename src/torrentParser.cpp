#include <torrentParser.hpp>
#include <torrent.hpp>
#include <istream>
#include <bencodeParserUtil.hpp>
#include <map>
#include <set>
#include <iostream>

using std::string;
using std::list;


namespace torrent {
    
    static std::set<string> getAnnounceList(const BencodeList* announceListToken)
    {
        std::set<string> announcelist;
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
            announcelist.insert(dynamic_cast<ByteStringToken *>(it)->getString());
        }
        return std::move(announcelist);
    }
    
    static void getInfo(const BencodeDict& infoDict, int& fileLength, std::string& name, int& pieceLength, std::vector<char>& pieces) {
        const IntegerToken &fileLengthToken  = dynamic_cast<const IntegerToken&>(infoDict["length"]);
        const IntegerToken &pieceLengthToken = dynamic_cast<const IntegerToken&>(infoDict["piece length"]);
        const ByteStringToken& nameToken     = dynamic_cast<const ByteStringToken&>(infoDict["name"]);
        const ByteStringToken& piecesToken   = dynamic_cast<const ByteStringToken&>(infoDict["pieces"]);
        name        = nameToken.getString();
        pieceLength = pieceLengthToken.getValue();
        fileLength  = fileLengthToken.getValue();
        pieces      = piecesToken.getValue();
    }

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeDict *ast;
        ByteStringToken *announceToken;
        std::set<string> announceList;
        int piece_length, fileLength;
        string name;
        std::vector<char> pieces;
        std::map<ByteStringToken, BencodeToken*, BencodeDict::BencodeDictComparator> torrentDict;
        
        if (in.bad()) {
            throw new ParseError("Bad stream");
        }
        ast = dynamic_cast<BencodeDict *>(&BencodeParserUtil::parseStream(in));
        if (nullptr == ast) {
            throw new ParseError("Illformed torrent file");
        }
        torrentDict = ast->getValues();
        announceToken = dynamic_cast<ByteStringToken *>(torrentDict["announce"]);
        announceList = getAnnounceList(dynamic_cast<BencodeList *>(
                                                                   torrentDict["announce-list"]));
        announceList.insert(announceToken->getString());
        getInfo(dynamic_cast<BencodeDict&>(*torrentDict["info"]), fileLength, name, piece_length, pieces);
        return std::move(t);
     }
}

