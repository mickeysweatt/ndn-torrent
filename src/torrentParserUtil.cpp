// torrentParserUtil.cpp                                               -*-C++-*-
#include <torrentParserUtil.hpp>
#include <bencodeParserUtil.hpp>
#include <torrent.hpp>
#include <istream>

#include <map>
#include <set>
#include <iostream>

#include <cassert>

using std::string;
using std::list;
using std::set;
using std::pair;
using std::vector;
using std::map;

//==============================================================================
//                    STATIC HELPER FUNCTION DECLARATIONS
//==============================================================================
namespace torrent {
    static bool isMultieFileTorrent(const BencodeDict& infoDict);
    
    static void getInfoCommon(const BencodeDict& infoDict,
                              std::string&       name,
                              int&               pieceLength,
                              std::vector<char>& pieces);

    static void getInfoMultiFileTorrent(const BencodeDict&        infoDict,
                                        set<pair<string, size_t>> files,
                                        string&                   name,
                                        int&                      pieceLength,
                                        vector<char>&             pieces);

    static void getInfoSingleFileTorrent(const BencodeDict& infoDict,
                                         int&               fileLength,
                                         std::string&       name,
                                         int&               pieceLength,
                                         std::vector<char>& pieces);

    static set<string> getAnnounceList(const BencodeList* announceListToken);
}

//==============================================================================
//                    STATIC HELPER FUNCTIONS DEFINITIONS
//==============================================================================
namespace torrent {
    static bool isMultieFileTorrent(const BencodeDict& infoDict) 
    {
       return infoDict.end() == infoDict.find("length");
    }
    
    static void getInfoCommon(const BencodeDict& infoDict,
                              std::string&       name,
                              int&               pieceLength,
                              std::vector<char>& pieces)
    {
        auto pieceLengthTokenPtr = 
                dynamic_cast<const BencodeIntegerToken    *>(
                                         infoDict.find("piece length")->second);
        auto nameTokenPtr       = 
                dynamic_cast<const BencodeByteStringToken *>(
                                         infoDict.find("name")->second);
        auto piecesTokenPtr      = 
                dynamic_cast<const BencodeByteStringToken *>(
                                          infoDict.find("pieces")->second);
        assert(nullptr != pieceLengthTokenPtr &&     
               nullptr != nameTokenPtr        && 
               nullptr != piecesTokenPtr);
        name        = nameTokenPtr->getString();
        pieceLength = pieceLengthTokenPtr->getValue();
        pieces      = piecesTokenPtr->getValue();
    }
    
    static void getInfoMultiFileTorrent(const BencodeDict&        infoDict,
                                        set<pair<string, size_t>> files,
                                        string&                   name,
                                        int&                      pieceLength,
                                        vector<char>&             pieces)
    {
        auto fileListToken  = dynamic_cast<const BencodeList *>(
                                                infoDict.find("files")->second);
        // each file here is a dict with meta-info about file
        for (auto fileMetaInfo : fileListToken->getTokens()) {
            auto fileDict = dynamic_cast<const BencodeDict&>(*fileMetaInfo);
            std::string pathString = "";
            auto path = dynamic_cast<const BencodeList *>(
                                                 fileDict.find("path")->second);
            auto pathCompnents = path->getTokens();
            // stringify the path name (path is a list of path components)
            for (auto pathComponent : *path) {
                pathString += dynamic_cast<const BencodeByteStringToken *>(
                                                  pathComponent)->getString() +
                            (pathComponent == pathCompnents.back() ? "" : "/" );
            }
            auto lengthToken = dynamic_cast<const BencodeIntegerToken *>(
                                   fileDict.find("length")->second);
            assert (nullptr != lengthToken);
            files.insert(std::pair<string, size_t>(pathString, 
                                                   lengthToken->getValue()));
        }
        getInfoCommon(infoDict, name, pieceLength, pieces);
    }
    
    static void getInfoSingleFileTorrent(const BencodeDict& infoDict,
                                         int&               fileLength,
                                         std::string&       name,
                                         int&               pieceLength,
                                         std::vector<char>& pieces)
    {
        getInfoCommon(infoDict, name, pieceLength, pieces);
        auto fileLengthToken  = dynamic_cast<const BencodeIntegerToken *>(
                                               infoDict.find("length")->second);
        assert(nullptr != fileLengthToken);
        pieceLength = fileLengthToken->getValue();
    }

    static set<string> getAnnounceList(const BencodeList* announceListToken)
    {
        std::set<string> announcelist;
        list<BencodeToken *> tempList1, tempList2;
        BencodeList *curr_list;

        assert(nullptr != announceListToken);

        tempList1 = announceListToken->getTokens();
        // unpack announce-list
        for (auto it : tempList1) {
            curr_list = dynamic_cast<BencodeList *>(it);
            assert(nullptr != curr_list);
            for (auto it2 : curr_list->getTokens()) {
                tempList2.push_back((it2));
            }
        }
        // grab strings from unpacked list
        for (auto it : tempList2) {
            auto announceURLToken = dynamic_cast<BencodeByteStringToken *>(it);
            assert(nullptr != announceURLToken);
            announcelist.insert(announceURLToken->getString());
        }
        return std::move(announcelist);
    }
}

namespace torrent {
//==============================================================================
//                           TorrentParserUtil
//==============================================================================
    Torrent TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeDict *ast;
        BencodeByteStringToken *announceToken;
        set<string> announceList;
        set<pair<string, size_t>> files;
        int piece_length, fileLength;
        string name;
        vector<char> pieces;
        map<BencodeByteStringToken, 
                 BencodeToken*, 
                 BencodeDict::BencodeDictComparator> torrentDict;
        
        if (!in.good()) {
            throw new ParseError("Bad stream");
        }
        ast = dynamic_cast<BencodeDict *>(&BencodeParserUtil::parseStream(in));
        assert(nullptr != ast);
        torrentDict = ast->getValues();
        announceToken = dynamic_cast<BencodeByteStringToken *>(
                                                       torrentDict["announce"]);
        announceList = getAnnounceList(dynamic_cast<BencodeList *>(
                                                 torrentDict["announce-list"]));
        // torrents are weird, and have an "announce" and "announce-list"
        announceList.insert(announceToken->getString());
        if (isMultieFileTorrent(dynamic_cast<BencodeDict&>(
                                                       *torrentDict["info"]))) 
        {
            getInfoMultiFileTorrent(
                               dynamic_cast<BencodeDict&>(*torrentDict["info"]), 
                                                          files, 
                                                          name, 
                                                          piece_length, 
                                                          pieces);
        }
        else {
            getInfoSingleFileTorrent(
                               dynamic_cast<BencodeDict&>(*torrentDict["info"]), 
                                                          fileLength, 
                                                          name, 
                                                          piece_length, 
                                                          pieces);
        }
        return std::move(t);
     }
}

