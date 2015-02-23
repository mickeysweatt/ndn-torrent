#include <torrent.hpp>
#include <chunkInfo.hpp>

namespace torrent {
	// TODO: Hannah
    Torrent::Torrent(std::list<std::string>& announceList,
                      std::string&            name,
                      size_t                  pieceLength,
                      size_t                  fileLength,
                      std::vector<char>       pieces)
     : m_name(name), m_pieceLength(pieceLength)
     {
		memcpy(&m_announceList, &announceList, sizeof(announceList));
     }
    
    Torrent::Torrent(std::list<std::string>&              announceList,
                     std::string&                         name,
                     size_t                               pieceLength,
                     const std::vector<const FileTuple&>& fileTuples,
                     const std::vector<char>&             pieces)
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

    const std::list<std::string>& Torrent::getAnnounceURLList() const
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
