#ifndef INCLUDED_TORRENT_HPP
#define INCLUDED_TORRENT_HPP

#include <list>
#include <string>
#include <chunkInfo.hpp>

namespace torrent {

class Chunk;

class Torrent {
    // A value-semantic attribute class for torrents. 
  public:
    // CREATORS
    Torrent();
    // Create an empty 'torrent'.

    ~Torrent();
    // Destroy this object.

    Torrent(std::list<std::string>& announceList, 
            std::string&            name,
            size_t                  pieceLength/*,
            std::vector<char>       pieces,
            std::list<...>          fileList */
           );
    
    // ACCESSORS
    const std::list<std::string>& getAnnounceURLList() const;
    // Return the list with all the the domains for all trackers for this
    // torrent

    const std::string& getName() const;
    // Return the name of this torrent file.

    size_t getPieceLength() const;
    // Return the length of each piece in this torrent.

    //const std::list<Chunk>& getChunks() const;
    // Return a list of all the chunks in this torrent.
  
  private:
    // DATA
    std::list<std::string>  m_announceList;
    std::string             m_name;
    size_t                  m_pieceLength;
    std::list<ChunkInfo>    m_chunks;
};

}
#endif