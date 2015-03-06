#include <cstring>
#include <chunkInfo.hpp>
#include <filePiece.hpp>
#include <sha1hash.hpp>

#include <iostream>

namespace torrent{

    ChunkInfo::ChunkInfo()
    {
    }

    ChunkInfo::~ChunkInfo()
    {
    }

    ChunkInfo::ChunkInfo(size_t id, unsigned char *hash)
    : m_id(id), m_hash(SHA1Hash(hash))
    {
    }

    void ChunkInfo::addFilePiece(const FilePiece& fp)
    {
        m_file_pieces.push_back(fp);
    }

    void ChunkInfo::setChunkId(size_t id)
    {
        m_id = id;
    }
    
    std::ostream& operator<<(std::ostream& s, const ChunkInfo& c)
    {
        s << "id: " << c.m_id ;
        return s;
    }
}
