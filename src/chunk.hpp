#ifndef INCLUDED_CHUNK_HPP
#define INCLUDED_CHUNK_HPP

#include <chunkInfo.hpp>
#include <memory>
#include <vector>

namespace torrent {

class Chunk {
   // A class that contains metadata and the actual data on a chunk.
public:
   // CREATORS
   Chunk() = default;
   // Creates an empty chunk.

   Chunk(const ChunkInfo& metadata, const std::vector<char>& buffer);
   // Creates a chunk and initializes the metadata and data.

   Chunk(ChunkInfo&& metadata, std::vector<char>&& buffer);
   // Creates a chunk by moving in metadata and data.

   Chunk& operator=(const Chunk& other) = default;

   ~Chunk() = default;
   // Destroys this object.

   // ACCESSORS
   const ChunkInfo& getMetadata() const;
   // Returns the metadata for this chunk.

   const std::vector<char>& getBuffer() const;
   // Returns the buffer containing the data for this chunk.

   // MUTATORS
   void setMetadata(const ChunkInfo& metadata);
   // Sets the metadata for this chunk.

   void setBuffer(const std::vector<char>& buffer);
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

inline Chunk::Chunk(const ChunkInfo& metadata, const std::vector<char>& buffer)
   : m_metadata(metadata), m_buffer(new std::vector<char>(buffer))
{
}

inline Chunk::Chunk(ChunkInfo&& metadata, std::vector<char>&& buffer)
   : m_metadata(std::move(metadata)), m_buffer(new std::vector<char>(std::move(buffer)))
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

inline void Chunk::setMetadata(const ChunkInfo& metadata)
{
   m_metadata = metadata;
}

inline void Chunk::setBuffer(const std::vector<char>& buffer)
{
   m_buffer = std::shared_ptr<std::vector<char>>(new std::vector<char>(buffer));
}

} // namespace torrent

#endif // INCLUDED_CHUNK_HPP
