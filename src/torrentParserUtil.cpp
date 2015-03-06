// torrentParserUtil.cpp                                               -*-C++-*-
#include <torrentParserUtil.hpp>
#include <bencodeParserUtil.hpp>
#include <torrent.hpp>
#include <istream>
#include <map>
#include <unordered_set>
#include <iostream>

#include <cassert>

using std::string;
using std::list;
using std::unordered_set;
using std::pair;
using std::vector;
using std::map;
using std::shared_ptr;
using std::dynamic_pointer_cast;

//==============================================================================
//                    STATIC HELPER FUNCTION DECLARATIONS
//==============================================================================
namespace torrent {
    static bool isMultiFileTorrent(const BencodeDict& infoDict);
    
    static void getInfoCommon(const BencodeDict& infoDict,
                              std::string&       name,
                              size_t&            pieceLength,
                              std::vector<char>& pieces);

    static void getInfoMultiFileTorrent(const BencodeDict&         infoDict,
                                        list<pair<string, size_t> > files,
                                        string&                    name,
                                        size_t&                    pieceLength,
                                        vector<char>&              pieces);

    static void getInfoSingleFileTorrent(const BencodeDict& infoDict,
                                         size_t&            fileLength,
                                         std::string&       name,
                                         size_t&            pieceLength,
                                         std::vector<char>& pieces);

    static 
    unordered_set<string> getAnnounceList(shared_ptr<BencodeList> announceListToken);
}

//==============================================================================
//                    STATIC HELPER FUNCTIONS DEFINITIONS
//==============================================================================
namespace torrent {
    static bool isMultiFileTorrent(const BencodeDict& infoDict) 
    {
       return infoDict.end() == infoDict.find("length");
    }
    
    static void getInfoCommon(const BencodeDict& infoDict,
                              std::string&       name,
                              size_t&               pieceLength,
                              std::vector<char>& pieces)
    {
        auto pieceLengthTokenPtr = 
                 dynamic_pointer_cast<const BencodeIntegerToken>(
                                         infoDict.find("piece length")->second);
        auto nameTokenPtr       = 
                 dynamic_pointer_cast<const BencodeByteStringToken>(
                                         infoDict.find("name")->second);
        auto piecesTokenPtr      = 
                 dynamic_pointer_cast<const BencodeByteStringToken >(
                                          infoDict.find("pieces")->second);
        assert(nullptr != pieceLengthTokenPtr &&     
               nullptr != nameTokenPtr        && 
               nullptr != piecesTokenPtr);
        name        = nameTokenPtr->getString();
        pieceLength = pieceLengthTokenPtr->getValue();
        pieces      = piecesTokenPtr->getValue();
    }
    
    static void getInfoMultiFileTorrent(const BencodeDict&        infoDict,
                                        list<pair<string, size_t> > files,
                                        string&                   name,
                                        size_t&                      pieceLength,
                                        vector<char>&             pieces)
    {
        auto fileListToken  = dynamic_pointer_cast<const BencodeList >(
                                                infoDict.find("files")->second);
        // each file here is a dict with meta-info about file
        for (auto fileMetaInfo : fileListToken->getTokens()) {
            auto fileDict = dynamic_pointer_cast<const BencodeDict>(
                                                                  fileMetaInfo);
            std::string pathString = "";

            auto path = dynamic_pointer_cast<const BencodeList>(
                                                fileDict->find("path")->second);
            auto pathCompnents = path->getTokens();
            // stringify the path name (path is a list of path components)
            for (auto pathComponent : *path) {
                pathString += 
                    dynamic_pointer_cast<const BencodeByteStringToken>(
                                                  pathComponent)->getString() +
                            (pathComponent == pathCompnents.back() ? "" : "/" );
            }

            auto lengthToken = 
                           dynamic_pointer_cast<const BencodeIntegerToken>(
                                   fileDict->find("length")->second);
            assert (nullptr != lengthToken);
            files.push_back(std::make_pair(pathString,
                                           lengthToken->getValue()));
        }
        getInfoCommon(infoDict, name, pieceLength, pieces);
    }
    
    static void getInfoSingleFileTorrent(const BencodeDict& infoDict,
                                         size_t&               fileLength,
                                         std::string&       name,
                                         size_t&               pieceLength,
                                         std::vector<char>& pieces)
    {
        getInfoCommon(infoDict, name, pieceLength, pieces);
        auto fileLengthToken  = 
                 dynamic_pointer_cast<const BencodeIntegerToken >(
                                               infoDict.find("length")->second);
        assert(nullptr != fileLengthToken);
        fileLength = fileLengthToken->getValue();
    }

    static 
    std::unordered_set<string> getAnnounceList(shared_ptr<BencodeList> announceListToken)
    {
        std::unordered_set<string> announcelist;
        std::shared_ptr<BencodeList> currList;
        assert(nullptr != announceListToken);
        // unpack announce-list
        for (auto it :  announceListToken->getTokens()) {
            currList = dynamic_pointer_cast<BencodeList>(it);
            assert(nullptr != currList);
            // grab strings from unpacked list
            for (auto it2 : currList->getTokens()) {
                auto announceURLToken = 
                              dynamic_pointer_cast<BencodeByteStringToken>(it2);
                assert(nullptr != announceURLToken);
                announcelist.insert(announceURLToken->getString());
            }
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
        shared_ptr<BencodeDict> ast;
        shared_ptr<BencodeByteStringToken>announceToken;
        unordered_set<string> announceList;
        list<pair<string, size_t>> files;

         size_t pieceLength, fileLength;
         string name;
         vector<char> pieces;
         map<BencodeByteStringToken, 
             shared_ptr<BencodeToken>, 
             BencodeDict::BencodeDictComparator> torrentDict;
        
         if (!in.good()) {
             throw ParseError("Bad stream");
         }
         ast = dynamic_pointer_cast<BencodeDict>(
                                            BencodeParserUtil::parseStream(in));
         assert(nullptr != ast);
         torrentDict = ast->getValues();
         announceToken = dynamic_pointer_cast<BencodeByteStringToken>(
                                                       torrentDict["announce"]);
         announceList = getAnnounceList(dynamic_pointer_cast<BencodeList>(
                                                 torrentDict["announce-list"]));
         // torrents are weird, and have an "announce" and "announce-list"
         announceList.insert(announceToken->getString());
         if (isMultiFileTorrent(dynamic_cast<BencodeDict&>(
                                                         *torrentDict["info"])))
         {
             getInfoMultiFileTorrent(
                               dynamic_cast<BencodeDict&>(*torrentDict["info"]),
                                                           files, 
                                                           name, 
                                                           pieceLength, 
                                                           pieces);
             t = Torrent(announceList,
                         name,
                         pieceLength,
                         files,
                         pieces);
         }
         else {
             getInfoSingleFileTorrent(
                               dynamic_cast<BencodeDict&>(*torrentDict["info"]), 
                                                           fileLength, 
                                                           name, 
                                                           pieceLength, 
                                                           pieces);
             t = Torrent(announceList, name, pieceLength, fileLength, pieces);
         }
        return std::move(t);
    }
}

