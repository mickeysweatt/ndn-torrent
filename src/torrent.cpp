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
		            size_t                  pieceLength)
     : m_name(name), m_pieceLength(pieceLength)
     {
		memcpy(&m_announceList, &announceList, sizeof(announceList));
     }


}