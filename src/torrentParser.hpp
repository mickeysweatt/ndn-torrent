#ifndef INCLUDED_TORRENT_PARSER_HPP
#define INCLUDED_TORRENT_PARSER_HPP

#include <string>
#include <exception>

namespace torrent {

class Torrent;

class TorrentParserUtil {
	// A utility class for parsing torrent files from disk into an instance
	// of a 'Torrent'. The format for a torrent file is specified here:
	// <https://en.wikipedia.org/wiki/Torrent_file>
  
  // TYPES
  public:
  	 class ParseError : public std::exception {
  	 	// An exception thrown if there if a malformed torrent file is detected
  	   public:
  	   	explicit ParseError(const std::string&  what);
  	 };

  	// CLASS METHODS
	static Torrent&& parseFile(const std::string& filename);
		// Parse the file referenceed by the specified 'filename' into a
		// 'Torrent'. Behavior undefined unless 'filename' references a valid
		// bencoded torrent file.
	
};

//==============================================================================
//							INLINE METHODS
//==============================================================================

 inline 
 TorrentParserUtil::ParseError::ParseError(const std::string &what) : exception()
 {
 }


} // end of torrent namespace

#endif