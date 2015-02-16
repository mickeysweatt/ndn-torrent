#ifndef INCLUDED_SEEDER_HPP
#define INCLUDED_SEEDER_HPP

#include <ndn-cxx/contexts/producer-context.hpp>

#include <chunk.hpp>
#include <torrentClientProto.hpp>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

/// TODO: Probably move this to a global header.
const char TORRENT_NAMESPACE[] = "/torrent";

namespace torrent {

class ChunkInfo;

class Seeder {
   // A class that repesents the seeding, i.e. uploading, portion of the client.
   // It will respond to interest packets for chunks that it can upload.
public:
   // CREATORS
	 explicit Seeder(TorrentClientProtocol& clientProtocol);
	 // Create a Seeder that communicates with clientProtocol.

   Seeder(TorrentClientProtocol& clientProtocol,
					const std::string& prefix);
   // Create a Seeder that communicates with clientProtocol, and seeds using the
	 // given prefix.

   ~Seeder();
   // Destroy this object.

   int upload(const std::list<Chunk>& chunkDataList);
   // Announce to the network to forward interest packets for chunks in
   // chunkDataList to this client.

   int stopUploading(const std::list<ChunkInfo>& chunkInfoList);
   // Announce to the network that this client no longer serves interest packets
   // for chunks in chunkDataList.

private:
   // DATA
   TorrentClientProtocol& m_clientProtocol;
	 std::unordered_map<size_t, std::unique_ptr<ndn::Producer>> m_producers;
	 std::unordered_map<size_t, Chunk> m_chunks;
	 std::string m_prefix;

	 // PRIVATE FUNCTIONS
	 void onInterest(ndn::Interest& interest);
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================

/// TODO: Ideally this function shouldn't exist, but need to consult with group
/// on changing the interface.
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
