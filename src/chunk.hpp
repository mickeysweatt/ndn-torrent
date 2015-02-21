#ifndef INCLUDED_CHUNK_HPP
#define INCLUDED_CHUNK_HPP

#include <chunkInfo.hpp>
#include <memory>
#include <vector>

namespace torrent {

class ChunkInfo;

class Chunk {
	 // A class that contains metadata and the actual data on a chunk.
public:
	 // CREATORS
	 Chunk();
	 // Creates an empty chunk.

	 Chunk(const ChunkInfo& metadata, const std::vector<char>& buffer);
	 // Creates a chunk and initializes the metadata and data.

	 ~Chunk();
	 // Destroys this object.

	 // ACCESSORS
	 const ChunkInfo& getMetadata() const;
	 // Returns the metadata for this chunk.

	 const std::vector<char>& getBuffer() const;
	 // Returns the buffer containing the data for this chunk.

	 // MUTATORS
	 int setMetadata(const ChunkInfo& metadata);
	 // Sets the metadata for this chunk.

	 int setBuffer(const std::vector<char>& buffer);
	 // Sets the data for this chunk.

private:
	 // DATA
	 ChunkInfo m_metadata;

	 std::shared_ptr<std::vector<char>> m_buffer;
	 /// TODO: For large files, we don't want to keep all buffers in memory.
	 /// Discuss with team, but it might be wise to implement a cache.
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================

inline Chunk::Chunk()
{
}

inline Chunk::Chunk(const ChunkInfo& metadata, const std::vector<char>& buffer)
	 : m_metadata(metadata), m_buffer(new std::vector<char>(buffer))
{
}

inline Chunk::~Chunk()
{
}

inline const ChunkInfo& Chunk::getMetadata() const
{
	 return m_metadata;
}

inline const std::vector<char>& Chunk::getBuffer() const
{
	 return *m_buffer;
}

inline int Chunk::setMetadata(const ChunkInfo& metadata)
{
	 m_metadata = metadata;
	 return -1;
}

inline int Chunk::setBuffer(const std::vector<char>& buffer)
{
	 m_buffer = std::shared_ptr<std::vector<char>>(new std::vector<char>(buffer));
	 return -1;
}

} // namespace torrent

#endif // INCLUDED_CHUNK_HPP
