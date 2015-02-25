#include <torrent.hpp>
#include <chunkInfo.hpp>
#include <filePiece.hpp>

namespace torrent {
	// TODO: Hannah
	// Single File
    Torrent::Torrent(const std::unordered_set<std::string>& announceList,
                     std::string&                     name,
                     size_t                           pieceLength,
                     size_t                           fileLength,
                     const std::vector<char>&         pieces)
     : m_announceList(announceList)
     , m_name(name)
     , m_pieceLength(pieceLength)
     {
         // TODO
     }
    
    // Multiple Files
    Torrent::Torrent(const std::unordered_set<std::string>& announceList,
                     std::string&                           name,
                     size_t                                 pieceLength,
                     const std::list<FileTuple>&          fileTuples,
                     const std::vector<char>&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    {
        // TODO
    }
    
    // MOVE
    Torrent::Torrent(std::unordered_set<std::string>&& announceList,
                     std::string&&                     name,
                     size_t                            pieceLength,
                     std::list<FileTuple>&&          files,
                     std::vector<char>&&               pieces)
    : m_announceList(announceList)
    , m_name(name)
    , m_pieceLength(pieceLength)
    {
        // TODO
    }
    
    Torrent::Torrent(Torrent&& other)
    : m_announceList(std::move(other.m_announceList)),
      m_name(std::move(other.m_name)),
      m_pieceLength(other.m_pieceLength),
      m_chunks(std::move(other.m_chunks))
    {
    }

    const std::unordered_set<std::string>& Torrent::getAnnounceURLList() const
    {
        return m_announceList;
    }
    
    const std::string& Torrent::getName() const
    {
        return m_name;
    }
    
    size_t Torrent::getPieceLength() const
    {
        return m_pieceLength;
    }
    
    const std::list<ChunkInfo>& Torrent::getChunks() const
    {
        return m_chunks;
    }
    
    // TODO: if the vector sticks around, change to const vector<...>&
    std::vector<Torrent::FileTuple> Torrent::getFileTuples() const
    {
        //TODO: a real implementation.
        return std::vector<Torrent::FileTuple>();
    }

    Torrent& Torrent::operator=(Torrent&& rhs)
    {
        if (&rhs != this) {
            m_announceList = std::move(rhs.m_announceList);
            m_name = std::move(rhs.m_name);
            m_pieceLength = rhs.m_pieceLength;
            m_chunks = std::move(rhs.m_chunks);
        }
        return *this;
    }
}
