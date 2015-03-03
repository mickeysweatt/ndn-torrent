#include <filePiece.hpp>

namespace torrent {
    FilePiece::FilePiece(std::string name, size_t start, size_t end,
                         size_t firstChunkId, size_t lastChunkId, size_t length)
    :m_name(name), m_start(start), m_end(end)
    , m_firstChunkId(firstChunkId), m_lastChunkId(lastChunkId), m_length(length)
	{
	}

	FilePiece::~FilePiece()
	{
	}
}