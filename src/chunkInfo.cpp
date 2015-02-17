#include <cstring>
#include <chunkInfo.hpp>
#include <sha1hash.hpp>

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




}
