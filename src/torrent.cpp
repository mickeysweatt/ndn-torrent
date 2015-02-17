#include <torrent.hpp>
#include <chunkInfo.hpp>

namespace torrent {
	Torrent::Torrent()
	{
	}

	Torrent::~Torrent()
	{
	}

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
}
