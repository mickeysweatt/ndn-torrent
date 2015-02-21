#ifndef INCLUDED_SEEDER_HPP
#define INCLUDED_SEEDER_HPP

#include <ndn-cxx/contexts/producer-context.hpp>

#include <chunk.hpp>
#include <torrentClientProto.hpp>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

namespace torrent {

class ChunkInfo;

class Seeder {
   // A class that repesents the seeding, i.e. uploading, portion of the client.
   // It will respond to interest packets for chunks that it can upload.
public:
   // CREATORS
	 explicit Seeder(TorrentClientProtocol& clientProtocol);
	 // DEPRECATED, use two-arg constructor instead

   Seeder(TorrentClientProtocol& clientProtocol,
					const std::string& prefix);
   // Create a Seeder that communicates with clientProtocol, and seeds using the
	 // given prefix.

   ~Seeder();
   // Destroy this object.

   int upload(const std::list<Chunk>& chunkDataList);
	 // PRECONDITION: Chunks agree with metadata hash
   // Announce to the network to forward interest packets for chunks in
   // chunkDataList to this client.

	 int upload(const Chunk& chunk);
	 // PRECONDITION: Chunks agree with metadata hash
	 // Announce to the network to forward interest packets for this chunk to this
	 // client.

   int stopUploading(const std::list<ChunkInfo>& chunkInfoList);
   // Announce to the network that this client no longer serves interest packets
   // for chunks in chunkDataList.

	 int stopUploading(const std::list<Chunk>& chunkList);
	 // Announce to the network that this client no longer serves interest packets
	 // for chunks in chunkList.

	 int stopUploading(const ChunkInfo& chunkInfo);
	 // Announce to the network that this client no longer serves interest packets
	 // for this chunk.

	 int stopUploading(const Chunk& chunk);
	 // Announce to the network that this client no longer serves interest packets
	 // for this chunk.

	 // ERROR CODES
	 static const int NO_ERROR = 0;
	 static const int CHUNK_NOT_FOUND = 1;
private:
   // DATA
   TorrentClientProtocol& m_clientProtocol;
	 std::unordered_map<size_t, std::unique_ptr<ndn::Producer>> m_producers;
	 std::unordered_map<size_t, Chunk> m_chunks;
	 std::string m_prefix;

	 // PRIVATE FUNCTIONS
	 void onInterest(const ndn::Interest& interest);
	 void onCacheMiss(const ndn::Interest& interest);
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================

// DEPRECATED, use two-arg constructor instead
inline Seeder::Seeder(TorrentClientProtocol& clientProtocol)
	 : m_clientProtocol(clientProtocol)
{
	 m_prefix = "/torrent/file/";
}

inline Seeder::Seeder(TorrentClientProtocol& clientProtocol,
											const std::string& prefix)
	 : m_clientProtocol(clientProtocol), m_prefix(prefix)
{
	 /// TODO: Parse prefix to ensure it's a valid prefix?
}

inline Seeder::~Seeder()
{
}

} // namespace torrent

#endif // INCLUDED_SEEDER_HPP
