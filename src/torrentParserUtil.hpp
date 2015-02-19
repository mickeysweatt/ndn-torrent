// torrentParserUtil.hpp                                               -*-C++-*-
#ifndef INCLUDED_TORRENT_PARSER_HPP
#define INCLUDED_TORRENT_PARSER_HPP

//@PURPOSE: Provide a parser for torrent files.
//
//@CLASSES:
//  TorrentParserUtil: parser for torrent files.
//
//@DESCRIPTION: This component defines a utility class 'BencodeParserUtil' for
// parsing torrent files from disk into an instance of a 'Torrent'.  The format
// for a torrent file is specified here:
//..
//     <https://en.wikipedia.org/wiki/Torrent_file>
//..
//

#include <string>
#include <stdexcept>

namespace torrent {

class Torrent;

class TorrentParserUtil {
  // A utility for parsing torrent files.

  public:
    // PUBLIC CLASS METHODS
    static Torrent parseFile(std::istream& in);
        // Parse the specified 'in' stream as a torrent file. Behavior is
        // undefined unless contents refers to a valid torrent file.

    // PUBLIC TYPES
     class ParseError : public std::logic_error {
       // An exception thrown if there if a malformed torrent file is detected
       public:
         explicit ParseError(const std::string& what);
          // Create a 'ParseError' with the specified 'what' string
     };

     // CREATORS 
     TorrentParserUtil() = delete;
        // Create an instance of a parser util. Note that this is declared 
        // deleted because an instance of a 'TorrentParserUtil' should never be
        // created.
};

//==============================================================================
//                          INLINE METHODS
//==============================================================================

inline 
TorrentParserUtil::ParseError::ParseError(const std::string &what)
: logic_error(what)
{
}

} // end of torrent namespace

#endif
