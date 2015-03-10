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
   // This class is non-copyable.
private:
   class SeederCallback
   // A class that handles callbacks for the producer. In particular, it will
   // handle interests that come in for the file that is being seeded.
   {
   public:
      SeederCallback(Seeder& seeder);
      // Creates a SeederCallback instance.

      void onCacheMiss(ndn::Producer& producer, const ndn::Interest& interest);
      // Handles callbacks for when an interest arrives but the data is not
      // cached in the network.
   private:
      Seeder& m_seeder;
   };

public:
   // ERROR CODES
   enum SeederError
   {
      NO_ERROR, CHUNK_NOT_FOUND
   };

   // CREATORS
   explicit Seeder(TorrentClientProtocol& clientProtocol);
   // DEPRECATED, use two-arg constructor instead

   Seeder(const ndn::Name& prefix,
          TorrentClientProtocol& clientProtocol);
   // PRECONDITION: prefix is a valid prefix that ends with a '/'
   // Create a Seeder that communicates with clientProtocol, and seeds using the
   // given prefix.

   Seeder(const Seeder& other) = delete;
   Seeder& operator=(const Seeder& other) = delete;
   // Seeder is non-copyable

//   Seeder(Seeder&& other);
   // Move constructor to move a Seeder.

//   Seeder& operator=(Seeder&& other);
   // Move assignment to move a Seeder.

   ~Seeder();
   // Destroy this object.

   SeederError upload(const std::list<Chunk>& chunkDataList);
   // PRECONDITION: Chunks agree with metadata hash
   // Announce to the network to forward interest packets for chunks in
   // chunkDataList to this client.

   SeederError upload(std::list<Chunk>&& chunkDataList);
   // PRECONDITION: Chunks agree with metadata hash
   // Announce to the network to forward interest packets for chunks in
   // chunkDataList to this client.

   SeederError upload(const Chunk& chunk);
   // PRECONDITION: Chunks agree with metadata hash
   // Announce to the network to forward interest packets for this chunk to this
   // client.

   SeederError upload(Chunk&& chunk);
   // PRECONDITION: Chunks agree with metadata hash
   // Announce to the network to forward interest packets for this chunk to this
   // client.

   SeederError stopUploading(const std::list<ChunkInfo>& chunkInfoList);
   // Announce to the network that this client no longer serves interest packets
   // for chunks in chunkDataList.

   SeederError stopUploading(const std::list<Chunk>& chunkList);
   // Announce to the network that this client no longer serves interest packets
   // for chunks in chunkList.

   SeederError stopUploading(const ChunkInfo& chunkInfo);
   // Announce to the network that this client no longer serves interest packets
   // for this chunk.

   SeederError stopUploading(const Chunk& chunk);
   // Announce to the network that this client no longer serves interest packets
   // for this chunk.
private:
   // DATA
   ndn::Name m_prefix;
   TorrentClientProtocol& m_clientProtocol;
   //std::unordered_map<size_t, std::unique_ptr<ndn::Producer>> m_producers;
   ndn::Producer m_producer;
   std::unordered_map<size_t, Chunk> m_chunks;
   SeederCallback *m_callback;
};

} // namespace torrent

#endif // INCLUDED_SEEDER_HPP
        
