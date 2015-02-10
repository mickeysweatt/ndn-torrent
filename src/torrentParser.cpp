#include "torrentParser.hpp"
#include "torrent.hpp"
#include <istream>
#include <bencodeParser.hpp>

using std::string;
using std::list;

namespace torrent {

    Torrent&& TorrentParserUtil::parseFile(std::istream& in)
    {
        Torrent t;
        BencodeToken *ast;
        BencodeSimpleToken* ast_s;
        BencodeList *ast_l;
        
        if (in.bad()) {
            throw new ParseError("Bad stream");
        }
        ast = &BencodeParser::parseStream(in);
        ast_s = dynamic_cast<BencodeSimpleToken*>(ast);
        ast_l = dynamic_cast<BencodeList *>(ast);
        //std::string s = ast_s->getValue();
        return std::move(t);
     }
}

