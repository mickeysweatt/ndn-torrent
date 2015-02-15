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
    
    static bool isMultieFileTorrent(const BencodeDict& infoDict) {
       return infoDict.end() == infoDict.find("length");
    }
    
    static void getInfo(const BencodeDict& infoDict,
                        int&               fileLength,
                        std::string&       name,
                        int&               pieceLength,
                        std::vector<char>& pieces)
    {
        auto fileLengthToken  = dynamic_cast<const IntegerToken    *>(infoDict.find("length")->second);
        auto pieceLengthToken = dynamic_cast<const IntegerToken    *>(infoDict.find("piece length")->second);
        auto nameToken        = dynamic_cast<const ByteStringToken *>(infoDict.find("name")->second);
        auto piecesToken      = dynamic_cast<const ByteStringToken *>(infoDict.find("pieces")->second);
        name        = nameToken->getString();
        pieceLength = pieceLengthToken->getValue();
        fileLength  = fileLengthToken->getValue();
        pieces      = piecesToken->getValue();
    }

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeDict *ast;
        ByteStringToken *announceToken;
        std::set<string> announceList;
        std::set<std::pair<std::string, size_t>> files;
        int piece_length, fileLength;
        string name;
        std::vector<char> pieces;
        std::map<ByteStringToken, BencodeToken*, BencodeDict::BencodeDictComparator> torrentDict;
        
        if (!in.good()) {
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
        if (isMultieFileTorrent(dynamic_cast<BencodeDict&>(*torrentDict["info"]))) {
            throw new TorrentParserUtil::ParseError("Implement me");
        }
        getInfo(dynamic_cast<BencodeDict&>(*torrentDict["info"]), fileLength, name, piece_length, pieces);
        return std::move(t);
     }
}

