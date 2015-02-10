#ifndef INCLUDED_CHUNK_HPP
#define INCLUDED_CHUNK_HPP

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

  std::vector<char>& getBuffer() const;
  // Returns the buffer containing the data for this chunk.

  // MUTATORS
  int setMetadata(ChunkInfo& metadata);
  // Sets the metadata for this chunk.

  int setBuffer(std::vector<char>& buffer);
  // Sets the data for this chunk.

private:
  // DATA
  ChunkInfo& m_metadata;
  std::vector<char> m_buffer;
};

} // namespace torrent

#endif // INCLUDED_CHUNK_HPP
