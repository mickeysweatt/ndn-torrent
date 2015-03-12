#include <filePiece.hpp>

namespace torrent {
    FilePiece::~FilePiece()
    {
    }
    
    FilePiece::FilePiece(std::string name, size_t offset, size_t length,
              size_t firstChunkId, size_t lastChunkId, size_t fileLen)
    : m_name(name), m_offset(offset), m_length(length)
    , m_firstChunkId(firstChunkId), m_lastChunkId(lastChunkId), m_fileLen(fileLen)
	{
	}
    
    void FilePiece::setOffset(size_t offset, size_t length)
    {
        m_offset = offset;
        m_length = length;
    }
}