#ifndef INCLUDED_CHUNKINFO_HPP
#define INCLUDED_CHUNKINFO_HPP

#include <list>
#include <sha1hash.hpp>

namespace torrent{

class ChunkInfo
{
public:
	// CREATORS
	ChunkInfo();

	ChunkInfo(size_t id, unsigned char *hash);
	// Creates an instance with m_id and m_hash initialized

	~ChunkInfo();
	// Destroys this object

	// ACCESSORS
	size_t getChunkId(); // Get Chunk Id

	SHA1Hash getChunkHash(); // Get Chunk Hash Value

	//std::list<FilePiece> getChunkFileList(); 
	//Returns a list of 'FilePiece' contained in the Chunk

	// MUTATORS

private:
	SHA1Hash m_hash;
	//std::list<FilePiece> m_file_pieces; // TODO: Hannah
	size_t m_id;
};

// INLINE ACCESSORS
inline size_t ChunkInfo::getChunkId() { return m_id; }
inline SHA1Hash ChunkInfo::getChunkHash() { return m_hash; }
//inline std::list<FilePiece> ChunkInfo::getChunkFileList() { return m_file_pieces; }

}



#endif // INCLUDED_CHUNKINFO_HPP