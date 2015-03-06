#ifndef INCLUDED_CHUNKINFO_HPP
#define INCLUDED_CHUNKINFO_HPP

#include <list>
#include <sha1hash.hpp>
#include <filePiece.hpp>

namespace torrent{

class ChunkInfo
{
public:
	// CREATORS
	ChunkInfo();

	ChunkInfo(size_t id, unsigned char *hash);
	// Creates an instance with m_id and m_hash initialized

  ChunkInfo(size_t id, SHA1Hash hash);
  // Creates an instance with m_id and m_hash initialized

	~ChunkInfo();
	// Destroys this object

	// ACCESSORS
	size_t getChunkId() const;     // Get Chunk Id

	SHA1Hash getChunkHash() const; // Get Chunk Hash Value

	std::list<FilePiece> getFilePieceList() const; // Get FilePiece List

	// MUTATORS
    void addFilePiece(const FilePiece& fp);
    
    void setChunkId(size_t id);

private:
	SHA1Hash m_hash;
	std::list<FilePiece> m_file_pieces;
	size_t m_id;
};

// INLINE ACCESSORS
inline size_t ChunkInfo::getChunkId() const { return m_id; }
inline SHA1Hash ChunkInfo::getChunkHash() const { return m_hash; }
inline std::list<FilePiece> ChunkInfo::getFilePieceList() const { return m_file_pieces; }

}



#endif // INCLUDED_CHUNKINFO_HPP
