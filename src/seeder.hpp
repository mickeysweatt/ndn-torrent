#ifndef INCLUDED_SEEDER_HPP
#define INCLUDED_SEEDER_HPP

// #include <ndn/producer-context.hpp>

#include <torrentClientProto.hpp>

#include <list>
#include <string>

static const std::string TORRENT_NAMESPACE = "/torrent/";

namespace ndn {
	 class Producer;
}

namespace torrent {

class Chunk;
class ChunkInfo;

class Seeder {
   // A class that repesents the seeding, i.e. uploading, portion of the client.
   // It will respond to interest packets for chunks that it can upload.
public:
   // CREATORS
   explicit Seeder(TorrentClientProtocol& clientProtocol);
   // Create a Seeder that communicates with clientProtocol.

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
	 // std::vector<ndn::Producer> m_producer;
};

//==============================================================================
//                       INLINE FUNCTION DEFINTIONS
//==============================================================================

inline Seeder::Seeder(TorrentClientProtocol& clientProtocol)
	 : m_clientProtocol(clientProtocol)
{
}

inline Seeder::~Seeder()
{
}

} // namespace torrent

#endif // INCLUDED_SEEDER_HPP
