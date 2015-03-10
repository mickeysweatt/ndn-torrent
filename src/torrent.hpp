#ifndef INCLUDED_TORRENT_HPP
#define INCLUDED_TORRENT_HPP

#include <list>
#include <unordered_set>
#include <vector>
#include <string>
#include <chunkInfo.hpp>
#include <iosfwd>

namespace torrent {

class Chunk;

class Torrent {
    
    typedef std::pair<std::string, size_t> FileTuple;
    
    // A value-semantic attribute class for torrents. 
  public:
    // CREATORS
    Torrent() = default;
    // Create an empty 'torrent'.

    ~Torrent() = default;
    // Destroy this object.
    
    Torrent(const Torrent& other) = default;

    Torrent(const std::unordered_set<std::string>& announceList,
            std::string&                           name,
            size_t                                 pieceLength,
            size_t                                 fileLength,
            const std::vector<char>&               pieces);
    
    Torrent(std::unordered_set<std::string>&& announceList,
            std::string&&                     name,
            size_t                            pieceLength,
            std::list<FileTuple>&&            files,
            std::vector<char>&&               pieces);

    Torrent(const std::unordered_set<std::string>& announceList,
            std::string&                     name,
            size_t                           pieceLength,
            const std::list<FileTuple>&      fileTuples,
            const std::vector<char>&         pieces);
    
    Torrent(Torrent&& other);
        // Move the value of the specified 'other' object into this
        // object.
    
    // ACCESSORS
    const std::unordered_set<std::string>& getAnnounceURLList() const;
    // Return the list with all the the domains for all trackers for this
    // torrent

    const std::string& getName() const;
    // Return the name of this torrent file.

    size_t getPieceLength() const;
    // Return the length of each piece in this torrent.

    const std::list<ChunkInfo>& getChunks() const;
    // Return a list of all the chunks in this torrent.
    
    // MANIPULATORS
    Torrent& operator=(Torrent&& rhs);
        // Assign the value of the specified 'rhs' object to this value.
    
    Torrent& operator=(const Torrent& rhs) = default;
    
    // HELPER
    friend std::ostream& operator<<(std::ostream& s, const Torrent& t);
    friend bool fileOffsetsTester(const std::list<FileTuple>& fileTuples, const size_t&                            pieceLength, const Torrent& t);
  
  private:
    // DATA
    std::unordered_set<std::string>  m_announceList;
    std::string             m_name;
    size_t                  m_pieceLength;
    std::list<ChunkInfo>    m_chunks;
    std::vector<char>       m_completeHash;
    
};

}
#endif
